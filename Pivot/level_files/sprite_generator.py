import bpy
import os
import numpy as np
import cv2
from math import sqrt, ceil

class PivotPanel(bpy.types.Panel):
    bl_label = "Pivot"
    bl_idname = "OBJECT_PT_pivot"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "Tools"

    def draw(self, context):
        layout = self.layout

        # File browser button
        layout.label(text="Output Directory:")
        row = layout.row()
        row.prop(context.scene, "sprite_directory", text="")
        row.operator("wm.path_open", text="", icon='FILE_FOLDER').filepath = bpy.path.abspath(context.scene.sprite_directory)

        # Text field for folder name
#        layout.label(text="Folder Name:")
        layout.prop(context.scene, "folder_name")

        # Start frame and end frame
        layout.prop(context.scene, "frame_start")
        layout.prop(context.scene, "frame_end")

        # Render button
        row = layout.row()
        row.operator("object.render_pivot_spritesheet", text="Render Spritesheet")
        
        # Create spritesheet button
        row = layout.row()
        row.operator("object.create_pivot_spritesheet", text="Create Spritesheet")
        
        row = layout.row()
        row.prop(context.scene, "passes")


class RenderPivotSpritesheet(bpy.types.Operator):
    bl_idname = "object.render_pivot_spritesheet"
    bl_label = "Render Spritesheet"

    def execute(self, context):
        output_directory = bpy.path.abspath(context.scene.sprite_directory)
        folder_name = context.scene.folder_name
        frame_start = context.scene.frame_start
        frame_end = context.scene.frame_end

        # Create the output folder if it doesn't exist
        folder_path = os.path.join(output_directory, folder_name)
        if not os.path.exists(folder_path):
            os.makedirs(folder_path)

        # Render the frames to the output folder
        scene = bpy.context.scene
        folder_name = context.scene.folder_name
        for pass_index in range(context.scene.passes):
            if pass_index == 0:
                bpy.context.scene.render.engine = 'BLENDER_EEVEE'
                bpy.data.linestyles["LineStyle"].color = (0, 0, 0)

            if pass_index == 1:
                bpy.context.scene.render.engine = 'BLENDER_WORKBENCH'
                bpy.data.linestyles["LineStyle"].color = (0.5, 0.5, 0.5)
                # Change some scene properties for the second render pass
#                scene.use_nodes = True
#                scene.render.image_settings.file_format = 'PNG'
                folder_path += '_normals'
                folder_name += '_normals'
                if not os.path.exists(folder_path):
                    os.makedirs(folder_path)

            for frame in range(frame_start, frame_end+1):
                scene.frame_set(frame)
                filepath = os.path.join(folder_path, "{:04d}.png".format(frame))
                scene.render.filepath = filepath
                bpy.ops.render.render(write_still=True)

            # Pack the frames into a spritesheet
            frame_paths = [os.path.join(folder_path, "{:04d}.png".format(frame)) for frame in range(frame_start, frame_end+1)]
            spritesheet_directory = bpy.path.abspath(context.scene.sprite_directory)
            spritesheet_path = os.path.join(spritesheet_directory, folder_name + ".png")
            pack_frames_into_spritesheet(frame_paths, spritesheet_path)
            
        bpy.context.scene.render.engine = 'BLENDER_EEVEE'
        bpy.data.linestyles["LineStyle"].color = (0, 0, 0)

        return {'FINISHED'}


class CreatePivotSpritesheet(bpy.types.Operator):
    bl_idname = "object.create_pivot_spritesheet"
    bl_label = "Create Spritesheet"

    def execute(self, context):
        output_directory = bpy.path.abspath(context.scene.sprite_directory)  # change here
        folder_name = context.scene.folder_name
        frame_start = context.scene.frame_start
        frame_end = context.scene.frame_end

        # Create the output folder if it doesn't exist
        folder_path = os.path.join(output_directory, folder_name)
        if not os.path.exists(folder_path):
            os.makedirs(folder_path)

        # Pack the frames into a spritesheet
        frame_paths = [os.path.join(folder_path, "{:04d}.png".format(frame)) for frame in range(frame_start, frame_end+1)]
        spritesheet_directory = bpy.path.abspath(context.scene.sprite_directory)
        spritesheet_path = os.path.join(spritesheet_directory, context.scene.folder_name + ".png")
        pack_frames_into_spritesheet(frame_paths, spritesheet_path)

        return {'FINISHED'}


def pack_frames_into_spritesheet(frame_paths, spritesheet_path):  # change here
    # Load all the frames into memory as OpenCV images
    images = [cv2.imread(filepath, cv2.IMREAD_UNCHANGED) for filepath in frame_paths]

    # Determine the size of the spritesheet
    num_frames = len(images)
    max_size = max([image.shape[0] for image in images] + [image.shape[1] for image in images])
    max_width = max([image.shape[1] for image in images])
    max_height = max([image.shape[0] for image in images])
    num_cols = ceil(sqrt(num_frames))
    num_rows = ceil(num_frames / num_cols)
    sprite_width = max_width
    sprite_height = max_height
    spritesheet_width = num_cols * sprite_width
    spritesheet_height = num_rows * sprite_height

    # Create the spritesheet image and copy the frames into it
    spritesheet_image = np.zeros((spritesheet_height, spritesheet_width, 4), dtype=np.uint8)
    for i, image in enumerate(images):
        col = i % num_cols
        row = i // num_cols
        x_offset = col * sprite_width
        y_offset = row * sprite_height
        resized_image = cv2.resize(image, (sprite_width, sprite_height))
        if resized_image.shape[2] == 3:
            alpha_channel = np.full((sprite_height, sprite_width, 1), 255, dtype=np.uint8)
            resized_image = np.concatenate((resized_image, alpha_channel), axis=2)
        spritesheet_image[y_offset:y_offset+sprite_height, x_offset:x_offset+sprite_width, :] = resized_image

    # Save the spritesheet to a file
    cv2.imwrite(spritesheet_path, spritesheet_image)

    # Free the memory used by the images
    del images[:]
    cv2.destroyAllWindows()


def register():
    bpy.utils.register_class(PivotPanel)
    bpy.utils.register_class(RenderPivotSpritesheet)
    bpy.utils.register_class(CreatePivotSpritesheet)
    bpy.types.Scene.sprite_directory = bpy.props.StringProperty(subtype='DIR_PATH')
    bpy.types.Scene.folder_name = bpy.props.StringProperty(name="Folder Name")
    bpy.types.Scene.passes = bpy.props.IntProperty(name="Render Passes")

def unregister():
    bpy.utils.unregister_class(PivotPanel)
    bpy.utils.unregister_class(RenderPivotSpritesheet)
    bpy.utils.unregister_class(CreatePivotSpritesheet)
    del bpy.types.Scene.sprite_directory
    del bpy.types.Scene.folder_name


if __name__ == "__main__":
    register()
