extends Control

@onready var texture_rect: TextureRect = $TextureRect
@onready var lbl_status: Label = $PainelUI/Layout/LblStatus
@onready var slider_octaves: HSlider = $PainelUI/Layout/SliderOctaves
@onready var slider_persistence: HSlider = $PainelUI/Layout/SliderPersistence
@onready var slider_lacunarity: HSlider = $PainelUI/Layout/SliderLacunarity

var gerador: PerlinNoise

var largura_tela: int = 800
var altura_tela: int = 600
var escala: float = 0.01
var offset_x: float = 0.0
var offset_y: float = 0.0

var octaves: int = 6
var persistence: float = 0.5
var lacunarity: float = 2.0

var precisa_atualizar: bool = false


func _ready() -> void:
	gerador = PerlinNoise.create_with_seed(42)
	
	slider_octaves.value = octaves
	slider_persistence.value = persistence
	slider_lacunarity.value = lacunarity
	
	slider_octaves.value_changed.connect(_on_parametros_alterados)
	slider_persistence.value_changed.connect(_on_parametros_alterados)
	slider_lacunarity.value_changed.connect(_on_parametros_alterados)
	$PainelUI/Layout/BtnGerarSemente.pressed.connect(_gerar_nova_semente)
	
	_renderizar_ruido()


func _input(event: InputEvent) -> void:
	# ZOOM (Scroll do Mouse)
	if event is InputEventMouseButton:
		if event.button_index == MOUSE_BUTTON_WHEEL_UP:
			escala *= 0.9
			precisa_atualizar = true
		elif event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
			escala *= 1.1
			precisa_atualizar = true
			
	if event is InputEventMouseMotion and Input.is_mouse_button_pressed(MOUSE_BUTTON_RIGHT):
		offset_x -= event.relative.x * escala
		offset_y -= event.relative.y * escala
		precisa_atualizar = true

	if event is InputEventKey and event.is_pressed():
		var vel_movimento = 20.0
		if event.keycode == KEY_W: offset_y -= vel_movimento
		elif event.keycode == KEY_S: offset_y += vel_movimento
		elif event.keycode == KEY_A: offset_x -= vel_movimento
		elif event.keycode == KEY_D: offset_x += vel_movimento
		elif event.keycode == KEY_SPACE: _gerar_nova_semente()
		precisa_atualizar = true


func _on_parametros_alterados(_valor_ignorado: float) -> void:
	octaves = int(slider_octaves.value)
	persistence = slider_persistence.value
	lacunarity = slider_lacunarity.value
	
	gerador.set_octaves(octaves)
	gerador.set_persistence(persistence)
	gerador.set_lacunarity(lacunarity)
	precisa_atualizar = true


func _process(_delta: float) -> void:
	if precisa_atualizar:
		_renderizar_ruido()
		precisa_atualizar = false


func _gerar_nova_semente() -> void:
	var nova_semente = randi()
	gerador = PerlinNoise.create_with_seed(nova_semente)
	_on_parametros_alterados(0.0)


func _renderizar_ruido() -> void:
	largura_tela = int(get_viewport_rect().size.x)
	altura_tela = int(get_viewport_rect().size.y)
	
	var tempo_inicio = Time.get_ticks_usec()
	
	var buffer_pixels: PackedByteArray = gerador.get_fbm_buffer(largura_tela, altura_tela, escala, offset_x, offset_y)
	
	var tempo_fim = Time.get_ticks_usec()
	var dt_ms = (tempo_fim - tempo_inicio) / 1000.0
	
	var img = Image.create_from_data(largura_tela, altura_tela, false, Image.FORMAT_RGBA8, buffer_pixels)
	texture_rect.texture = ImageTexture.create_from_image(img)
	
	lbl_status.text = "Tempo C++: %.2f ms\nEscala: %.4f\nOffX: %.1f | OffY: %.1f" % [dt_ms, escala, offset_x, offset_y]
	$PainelUI/Layout/LblOctaves.text = "Octaves: %d" % octaves
	$PainelUI/Layout/LblPersistence.text = "Persistence: %.2f" % persistence
	$PainelUI/Layout/LblLacunarity.text = "Lacunarity: %.2f" % lacunarity
