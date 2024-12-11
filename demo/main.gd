extends Node2D


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	var cammm = RealSense.new()
	cammm.add(5)
	cammm.add(7)
	print(cammm.get_total())
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass
