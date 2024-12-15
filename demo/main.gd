extends Node2D

var is_initialized = false
var is_capturing = false
var is_new_frame_ready = false
var image : Image

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	print("started")
	image = Image.create(640, 480, false, Image.FORMAT_RG8)
	pass # Replace with function body.


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta: float) -> void:
	if not is_capturing:
		$RealSense.start_capturing()
		is_capturing = true
	if is_new_frame_ready:
		image.set_data(640, 480, false, Image.FORMAT_RG8, $RealSense.get_depth_data())
		$TextureRect.texture = ImageTexture.create_from_image(image)
		is_new_frame_ready = false
	pass


func _on_button_pressed() -> void:
	#print($RealSense.get_size())
	#print($RealSense.get_depth_data())
	pass # Replace with function body.


func _on_realsense_initialized() -> void:
	is_initialized = true
	print("initialized")
	pass # Replace with function body.


func _on_realsense_new_depth_data(data: PackedByteArray) -> void:
	print(data[57484])
	#image.set_data(640, 480, false, Image.FORMAT_RG8, data)
	pass # Replace with function body.


func _on_realsense_new_frame_ready() -> void:
	print("new frame ready")
	is_new_frame_ready = true
	pass # Replace with function body.
