extends HBoxContainer

# --- UI REFERENCES ---
@onready var mesh_instance: MeshInstance3D = $"ViewportContainer/3DEnvironment/TerrainMesh"
@onready var generate_button: Button = $SettingsPanel/SettingsVBox/GenerateTerrainButton
@onready var camera: Camera3D = $"ViewportContainer/3DEnvironment/Camera3D"

# Sliders
@onready var octaves_slider: HSlider = $SettingsPanel/SettingsVBox/OctavesSlider
@onready var persistence_slider: HSlider = $SettingsPanel/SettingsVBox/PersistenceSlider
@onready var lacunarity_slider: HSlider = $SettingsPanel/SettingsVBox/LacunaritySlider
@onready var amplitude_slider: HSlider = $SettingsPanel/SettingsVBox/AmplitudeSlider

# Labels
@onready var octaves_label: Label = $SettingsPanel/SettingsVBox/OctavesLabel
@onready var persistence_label: Label = $SettingsPanel/SettingsVBox/PersistenceLabel
@onready var lacunarity_label: Label = $SettingsPanel/SettingsVBox/LacunarityLabel
@onready var amplitude_label: Label = $SettingsPanel/SettingsVBox/AmplitudeLabel

# --- MEMORY STATE ---
var perlin: PerlinNoise

# --- ORBIT CAMERA STATE ---
var camera_target: Vector3 = Vector3(0, 0, 0)
var camera_distance: float = 80.0
var camera_yaw: float = PI / 4.0   # Inicia inclinada em 45 graus no eixo Y
var camera_pitch: float = -PI / 6.0 # Inicia olhando 30 graus para baixo
var orbit_sensitivity: float = 0.005
var pan_sensitivity: float = 0.05
var zoom_speed: float = 5.0

func _ready() -> void:
	perlin = PerlinNoise.create_with_seed(42)
	
	generate_button.pressed.connect(_on_generate_button_pressed)
	
	octaves_slider.value_changed.connect(_on_parameters_changed)
	persistence_slider.value_changed.connect(_on_parameters_changed)
	lacunarity_slider.value_changed.connect(_on_parameters_changed)
	amplitude_slider.value_changed.connect(_on_parameters_changed)
	
	_update_ui_texts()
	_generate_3d_terrain()
	
	# Força o posicionamento inicial da câmera antes do primeiro frame
	_update_camera_transform()

# --- INPUT HANDLING (Event-Driven) ---

func _input(event: InputEvent) -> void:
	# 1. ORBIT (Girar a malha): Botão Esquerdo + Arrastar
	if event is InputEventMouseMotion and Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
		camera_yaw -= event.relative.x * orbit_sensitivity
		camera_pitch -= event.relative.y * orbit_sensitivity
		
		# Trava matemática polar (Gimbal Lock) a aproximadamente 89 graus
		camera_pitch = clamp(camera_pitch, -PI / 2.0 + 0.01, PI / 2.0 - 0.01)
		_update_camera_transform()
		
	# 2. PAN (Mover a malha lateralmente): Botão Direito + Arrastar
	elif event is InputEventMouseMotion and Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT):
		var right_dir = camera.transform.basis.x
		var up_dir = camera.transform.basis.y
		
		# Movemos o ponto focal inversamente ao movimento do mouse
		camera_target -= right_dir * event.relative.x * pan_sensitivity
		camera_target += up_dir * event.relative.y * pan_sensitivity
		_update_camera_transform()
		
	# 3. ZOOM (Aproximar/Afastar): Roda do Mouse
	elif event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_WHEEL_UP:
			camera_distance = max(5.0, camera_distance - zoom_speed)
			_update_camera_transform()
		elif event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
			camera_distance = min(200.0, camera_distance + zoom_speed)
			_update_camera_transform()

# Calcula a posição cartesiana a partir das coordenadas esféricas
func _update_camera_transform() -> void:
	var offset = Vector3(0, 0, camera_distance)
	# A ordem de rotação é estrita: Pitch local primeiro, Yaw global depois
	offset = offset.rotated(Vector3.RIGHT, camera_pitch)
	offset = offset.rotated(Vector3.UP, camera_yaw)
	
	camera.position = camera_target + offset
	camera.look_at(camera_target) # Garante que a lente aponte sempre para o centroide

# --- TERRAIN GENERATION LOGIC ---

func _on_generate_button_pressed() -> void:
	perlin.set_seed(randi())
	_generate_3d_terrain()

func _update_ui_texts() -> void:
	octaves_label.text = "Octaves: %d" % octaves_slider.value
	persistence_label.text = "Persistence: %.2f" % persistence_slider.value
	lacunarity_label.text = "Lacunarity: %.2f" % lacunarity_slider.value
	amplitude_label.text = "Amplitude: %.1f" % amplitude_slider.value

func _on_parameters_changed(_ignored_value: float) -> void:
	_update_ui_texts()
	_generate_3d_terrain()

func _generate_3d_terrain() -> void:
	perlin.set_octaves(int(octaves_slider.value))
	perlin.set_persistence(persistence_slider.value)
	perlin.set_lacunarity(lacunarity_slider.value)
	
	var amplitude = amplitude_slider.value
	var scale_factor = 0.05
	var grid_size = 100
	
	var surface_tool = SurfaceTool.new()
	surface_tool.begin(Mesh.PRIMITIVE_TRIANGLES)
	
	for z in range(grid_size):
		for x in range(grid_size):
			var y_height = perlin.get_fbm_2D(x * scale_factor, z * scale_factor) * amplitude
			surface_tool.set_uv(Vector2(x, z) / float(grid_size))
			surface_tool.add_vertex(Vector3(x, y_height, z))
	
	for z in range(grid_size - 1):
		for x in range(grid_size - 1):
			var i = x + z * grid_size
			
			surface_tool.add_index(i)
			surface_tool.add_index(i + 1)
			surface_tool.add_index(i + grid_size)
			
			surface_tool.add_index(i + 1)
			surface_tool.add_index(i + grid_size + 1)
			surface_tool.add_index(i + grid_size)
	
	surface_tool.generate_normals()
	
	var terrain_material = StandardMaterial3D.new()
	terrain_material.albedo_color = Color(0.25, 0.4, 0.2)
	terrain_material.roughness = 0.8
	surface_tool.set_material(terrain_material)
	
	mesh_instance.mesh = surface_tool.commit()
	
	# Ao centrar a malha na origem, o nosso `camera_target = Vector3(0,0,0)` aponta 
	# perfeitamente para o centro geométrico do terreno.
	mesh_instance.position = Vector3(-grid_size / 2.0, 0, -grid_size / 2.0)
