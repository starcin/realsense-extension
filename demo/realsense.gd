extends RealSense


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	print("starting init")
	self.initialize(640, 480, true, true)
	self.configure_depth_cropping(0.1, 2.0, -0.5, 0.5, -0.5, 0.5, false)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	pass


func _on_initialized() -> void:
	print(self.get_size())
	pass # Replace with function body.
