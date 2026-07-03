extends TextureRect

var gerador: PerlinNoise

func _ready() -> void:
	gerador = PerlinNoise.new()
	gerador.set_seed(42) 
	gerador.set_octaves(6)
	gerador.set_persistence(0.5)
	gerador.set_lacunarity(2.0)
	
	_renderizar_ruido()

func _renderizar_ruido() -> void:
	var largura = 256
	var altura = 256
	var escala = 0.01
	
	var buffer_pixels: PackedByteArray = gerador.get_fbm_buffer(largura, altura, escala, 0.0, 0.0)
	
	var img = Image.create_from_data(largura, altura, false, Image.FORMAT_RGBA8, buffer_pixels)
	
	texture = ImageTexture.create_from_image(img)
