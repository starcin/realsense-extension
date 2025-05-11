extends Node3D

var is_initialized = false
var is_capturing = false
var is_new_frame_ready = false
var depth_image : Image
var color_image : Image
var vertices_data : Image
var mesh_points : ArrayMesh


# Called when the node enters the scene tree for the first time.
func _ready() -> void:
	print("started")
	depth_image = Image.create(640, 480, false, Image.FORMAT_RG8)
	color_image = Image.create(640, 480, false, Image.FORMAT_RGB8)
	vertices_data = Image.create(640, 480, false, Image.FORMAT_RGBF)

	# Create point cloud mesh
	var points = []
	for y in range(480):
		for x in range(640):
			points.push_back(Vector3(
				float(x) / 640.0 * 1.6 - 0.8,
				float(y) / 480.0 * 1.2 - 0.6,
				0.0
			))

	var arrays = []
	arrays.resize(Mesh.ARRAY_MAX)  # Initialize the array with correct size
	arrays[Mesh.ARRAY_VERTEX] = PackedVector3Array(points)

	mesh_points = ArrayMesh.new()
	mesh_points.add_surface_from_arrays(Mesh.PRIMITIVE_POINTS, arrays)

	# Create and assign shader material
	var material = ShaderMaterial.new()
	material.shader = preload("res://vertex.gdshader")
	$MeshInstance3D2.mesh = mesh_points
	$MeshInstance3D2.material_override = material
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
	$MeshInstance3D.get_active_material(0).set_shader_parameter("depth_texture", texture)


func _on_realsense_new_color_data(data: PackedByteArray) -> void:
	color_image.set_data(640, 480, false, Image.FORMAT_RGB8, data)
	$CanvasLayer/ColorTextureRect.texture = ImageTexture.create_from_image(color_image)
	pass # Replace with function body.


func _on_real_sense_new_vertices_data(data: PackedVector3Array) -> void:
	# Convert the entire Vector3Array to bytes directly
	var bytes = data.to_byte_array()

	vertices_data.set_data(640, 480, false, Image.FORMAT_RGBF, bytes)
	var texture : ImageTexture = ImageTexture.create_from_image(vertices_data)
	$MeshInstance3D2.get_active_material(0).set_shader_parameter("vertices_data", texture)
