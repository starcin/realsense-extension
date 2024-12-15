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
	pass


func _on_realsense_initialized() -> void:
	is_initialized = true
	print("initialized")
	pass # Replace with function body.


func _on_realsense_new_depth_data(data: PackedByteArray) -> void:
	image.set_data(640, 480, false, Image.FORMAT_RG8, data)
	$TextureRect.texture = ImageTexture.create_from_image(image)
	pass # Replace with function body.
