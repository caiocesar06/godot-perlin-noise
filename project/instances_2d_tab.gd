extends HBoxContainer

# --- UI REFERENCES ---
@onready var screens_grid: GridContainer = $VisualizationScroll/ScreensGrid
@onready var quantity_spinbox: SpinBox = $SettingsPanel/SettingsVBox/QuantityHBox/QuantitySpinBox
@onready var random_seed_checkbox: CheckBox = $SettingsPanel/SettingsVBox/RandomSeedCheckBox
@onready var seed_spinbox: SpinBox = $SettingsPanel/SettingsVBox/SeedHBox/SeedSpinBox
@onready var generate_button: Button = $SettingsPanel/SettingsVBox/GenerateButton

@onready var octaves_slider: HSlider = $SettingsPanel/SettingsVBox/OctavesSlider
@onready var octaves_label: Label = $SettingsPanel/SettingsVBox/OctavesLabel
@onready var persistence_slider: HSlider = $SettingsPanel/SettingsVBox/PersistenceSlider
@onready var persistence_label: Label = $SettingsPanel/SettingsVBox/PersistenceLabel
@onready var lacunarity_slider: HSlider = $SettingsPanel/SettingsVBox/LacunaritySlider
@onready var lacunarity_label: Label = $SettingsPanel/SettingsVBox/LacunarityLabel

# --- MEMORY STATE ---
var active_generators: Array[PerlinNoise] = []
var active_screens: Array[TextureRect] = []

var internal_resolution: int = 256

func _ready() -> void:
	generate_button.pressed.connect(_on_generate_button_pressed)
	
	random_seed_checkbox.toggled.connect(_on_random_seed_checkbox_toggled)
	_on_random_seed_checkbox_toggled(random_seed_checkbox.button_pressed)
	
	octaves_slider.value_changed.connect(_on_parameters_changed)
	persistence_slider.value_changed.connect(_on_parameters_changed)
	lacunarity_slider.value_changed.connect(_on_parameters_changed)
	
	_update_ui_texts()

func _on_random_seed_checkbox_toggled(is_enabled: bool) -> void:
	seed_spinbox.editable = not is_enabled
	seed_spinbox.modulate.a = 0.5 if is_enabled else 1.0

func _update_ui_texts() -> void:
	octaves_label.text = "Octaves: %d" % octaves_slider.value
	persistence_label.text = "Persistence: %.2f" % persistence_slider.value
	lacunarity_label.text = "Lacunarity: %.2f" % lacunarity_slider.value

func _clear_memory() -> void:
	for child in screens_grid.get_children():
		screens_grid.remove_child(child)
		child.queue_free()
	
	active_generators.clear()
	active_screens.clear()

func _on_generate_button_pressed() -> void:
	_clear_memory()
	
	var active_count = int(quantity_spinbox.value)
	var is_random = random_seed_checkbox.button_pressed
	var base_seed = int(seed_spinbox.value)
	
	screens_grid.columns = mini(active_count, 3)
	
	for i in range(active_count):
		var instance_seed = randi() if is_random else (base_seed + i)
		
		var new_screen = TextureRect.new()
		new_screen.expand_mode = TextureRect.EXPAND_IGNORE_SIZE
		new_screen.stretch_mode = TextureRect.STRETCH_SCALE
		new_screen.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		new_screen.size_flags_vertical = Control.SIZE_EXPAND_FILL
		new_screen.custom_minimum_size = Vector2(0, internal_resolution)
		
		var generator = PerlinNoise.create_with_seed(instance_seed)
		
		active_generators.append(generator)
		active_screens.append(new_screen)
		screens_grid.add_child(new_screen)
		
	_recalculate_visual_noises()

func _on_parameters_changed(_ignored_value: float) -> void:
	_update_ui_texts()
	
	if active_generators.size() > 0:
		_recalculate_visual_noises()

func _recalculate_visual_noises() -> void:
	var cfg_octaves = int(octaves_slider.value)
	var cfg_persistence = persistence_slider.value
	var cfg_lacunarity = lacunarity_slider.value
	var scale_factor = 0.01
	
	for i in range(active_generators.size()):
		var generator = active_generators[i]
		var screen = active_screens[i]
		
		generator.set_octaves(cfg_octaves)
		generator.set_persistence(cfg_persistence)
		generator.set_lacunarity(cfg_lacunarity)
		
		var buffer = generator.get_fbm_buffer(internal_resolution, internal_resolution, scale_factor, 0.0, 0.0)
		var img = Image.create_from_data(internal_resolution, internal_resolution, false, Image.FORMAT_RGBA8, buffer)
		screen.texture = ImageTexture.create_from_image(img)
