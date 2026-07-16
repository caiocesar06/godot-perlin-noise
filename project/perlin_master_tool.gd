extends Control

@onready var tab_container: TabContainer = $TabContainer

func _ready() -> void:
	tab_container.set_tab_title(0, "Instâncias 2D")
	tab_container.set_tab_title(1, "Volumétrico 3D")
