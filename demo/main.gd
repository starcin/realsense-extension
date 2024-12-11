extends Node2D

var is_initialized = false

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	print("started")
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	#if is_initialized:
		#var data = camera.get_depth_data()
		#print("1")
	pass


func _on_button_pressed() -> void:
	#print($RealSense.get_size())
	#print($RealSense.get_depth_data())
	pass # Replace with function body.


func _on_realsense_initialized() -> void:
	print("initialized")
	pass # Replace with function body.
