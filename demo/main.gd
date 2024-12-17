extends Node3D

var is_initialized = false
var is_capturing = false
var is_new_frame_ready = false
var depth_image : Image
var color_image : Image

# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	print("started")
	depth_image = Image.create(640, 480, false, Image.FORMAT_RG8)
	color_image = Image.create(640, 480, false, Image.FORMAT_RGB8)
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
	depth_image.set_data(640, 480, false, Image.FORMAT_RG8, data)
	var texture : ImageTexture = ImageTexture.create_from_image(depth_image)
	$CanvasLayer/DepthTextureRect.texture = texture
	#$MeshInstance3D.set_instance_shader_parameter("depth_texture", texture)
	$MeshInstance3D.get_active_material(0).set_shader_parameter("depth_texture", texture)
	pass # Replace with function body.


func _on_realsense_new_color_data(data: PackedByteArray) -> void:
	color_image.set_data(640, 480, false, Image.FORMAT_RGB8, data)
	$CanvasLayer/ColorTextureRect.texture = ImageTexture.create_from_image(color_image)
	pass # Replace with function body.
