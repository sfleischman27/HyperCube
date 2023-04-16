import bpy
import json
import os
from os import path
from mathutils import Vector



class ExportJsonOperator(bpy.types.Operator):
    bl_idname = "pivot.json_operator"
    bl_label = "Pivot Json Operator"
    
    def execute(self, context):
        scene_scale = bpy.context.scene.scene_scale
          
        for object in bpy.data.objects:
            object.select_set(False)        
    
        # make sure the choosen assets folder exists
        dir = bpy.context.scene.assets_dir
        if not path.isdir(dir):
            raise Exception("the provided assets dir does not exist")
        else:
            print("assets folder successfully found")
        
        pack = bpy.context.scene.level_pack
        self.checkFileStructure(dir, pack)
        
        #init a dict to become the json
        json_dict = dict()
        
        #init a dictionary of data which will update assests.json
        ass_up = dict()
        ass_up["jsons"] = dict()
        ass_up["textures"] = dict()
        
        #Set name to pack plus suffix
        name = pack+ "_" + str(bpy.context.scene.suffix).zfill(4)
        json_dict["level_id"] = name            
        
        # get start and exit data
        start = bpy.context.scene.start
        exit = bpy.context.scene.exit
        
        if start.type != 'EMPTY': raise Exception("the start should be an empty")
        else: 
            json_dict["player_loc"] = [
                start.location.x * scene_scale,
                start.location.y * scene_scale,
                start.location.z * scene_scale
                ]
            
            #extract normal from the z axis of the start location
            matrix_world = start.matrix_world
            vec = Vector((round(matrix_world[0][2]), 
                round(matrix_world[1][2]), 
                round(matrix_world[2][2])
                ))
            vec.normalize()
            json_dict["norm"] = [vec[0],vec[1],vec[2]]
            
        if exit.type != 'EMPTY': raise Exception("the exit should be an empty")
        else: json_dict["exit"] = [
            exit.location.x * scene_scale,
            exit.location.y * scene_scale,
            exit.location.z * scene_scale
            ]
        
        #get and set the glowstick count   
        json_dict["glowsticks"] = bpy.context.scene.glow_count
        
        #initialize a dictionary to hold sprite and light data
        json_dict["sprites"] = dict()
                
        #get sprite data
        collectibles = bpy.context.scene.collectible_sprites
        col_dict = dict()
        
        sprite_count = 0
        for sprite in collectibles.objects:
            
            if sprite.parent is None:
                if not path.isfile(dir+ f"textures/{sprite.active_material.name}.png"):
                    raise Exception(f"{sprite.active_material.name}.png was not found in the assets/textures/ directory. Please add it.")
                
                sprite_dict = dict()
                sprite_dict["loc"] = [sprite.location.x * scene_scale,sprite.location.y * scene_scale,sprite.location.z * scene_scale]
                sprite_dict["tex"] = sprite.active_material.name
                sprite_dict["color"] = None
                sprite_dict["intense"] = None
                sprite_dict["collectible"] = True
                
                for ch in sprite.children:
                    if ch.data.type  == 'POINT':
                        sprite_dict["color"] = [ch.data.color.r, ch.data.color.g, ch.data.color.b]
                        sprite_dict["intense"] = ch.data.energy
                
                col_dict[f"{sprite_count}"] = sprite_dict
                sprite_count += 1
                
                #queue texture to be added to assets    
                ass_up["textures"][f"{sprite.active_material.name}"] = {"file":f"textures/{sprite.active_material.name}.png"}
            
        json_dict["sprites"].update(col_dict)
        
        
        # do the same for the decorative sprite collection
        decorations = bpy.context.scene.decorative_sprites
        deco_dict = dict()
        
        for sprite in decorations.objects:
            
            if sprite.parent is None:
                if not path.isfile(dir+ f"textures/{sprite.active_material.name}.png"):
                    raise Exception(f"{sprite.active_material.name}.png was not found in the assets/textures/ directory. Please add it.")
                
                sprite_dict = dict()
                sprite_dict["loc"] = [sprite.location.x * scene_scale,sprite.location.y * scene_scale,sprite.location.z * scene_scale]
                sprite_dict["tex"] = sprite.active_material.name
                sprite_dict["color"] = None
                sprite_dict["intense"] = None
                sprite_dict["collectible"] = False
                
                for ch in sprite.children:
                    if ch.data.type == 'POINT':
                        sprite_dict["color"] = [ch.data.color.r, ch.data.color.g, ch.data.color.b]
                        sprite_dict["intense"] = ch.data.energy
                
                json_dict["sprites"][f"{sprite_count}"] = sprite_dict
                sprite_count += 1
                
                #queue texture to be added to assets    
                ass_up["textures"][f"{sprite.active_material.name}"] = {"file":f"textures/{sprite.active_material.name}.png"}
          
                    
        
        #get solo point light data and make sprites wo textures
        point_lights = bpy.context.scene.point_lights
        
        for obj in point_lights.objects:
            if obj.data.type == 'POINT':
                
                sprite_dict = dict()
                sprite_dict["loc"] = [obj.location.x * scene_scale, obj.location.y * scene_scale, obj.location.z * scene_scale]
                sprite_dict["tex"] = None
                sprite_dict["color"] = [obj.data.color.r,obj.data.color.g,obj.data.color.b]
                sprite_dict["intense"] = obj.data.energy
                sprite_dict["collectible"] = False
                
                json_dict["sprites"][f"{sprite_count}"] = sprite_dict
                sprite_count += 1        
        
        #export the collision meshes as an obj
        fpath = dir+ f"meshes/{pack}/{name}_col.obj"
        collection = bpy.context.scene.collision_meshes
        self.export_collection_for_pivot(collection, fpath)
        json_dict["collision_mesh"] = f"meshes/{pack}/{name}_col.obj"
        
        #export the decorative meshes as an obj
        fpath = dir+ f"meshes/{pack}/{name}_dec.obj"
        collection = bpy.context.scene.decorative_meshes
        self.export_collection_for_pivot(collection, fpath)
        json_dict["render_mesh"] = f"meshes/{pack}/{name}_dec.obj"
        
        print(json_dict)
        # write the json
        json_path = dir+f"json/level_json/{pack}/{name}.json"
        ass_up["jsons"][name] = f"json/level_json/{pack}/{name}.json"
        with open(json_path, 'w') as file:
            json.dump(json_dict, file, indent = 4)
            
        #open the assets json and update contents
        
        with open(dir+"json/assets.json", 'r') as file:
            asset_dict = json.load(file)
            asset_dict["jsons"].update(ass_up["jsons"])
            asset_dict["textures"].update(ass_up["textures"])
        
        # replace the old assets with updated assets
        #NOTE user is responsible for deleting assets for old levels
        with open(dir+"json/assets.json", 'w') as file:
            json.dump(asset_dict,file, indent = 4)
        
        return {'FINISHED'}
    
    
    def checkFileStructure(self,dir,pack):
        # make sure necessary subdirectories exist
        if not path.isdir(dir+"json/"):
            os.mkdir(dir+"json/")
        if not path.isdir(dir+"json/level_json/"):
            os.mkdir(dir+"json/level_json/")
        if not path.isdir(dir+f"json/level_json/{pack}/"):
            os.mkdir(dir+f"json/level_json/{pack}/")
        
        if not path.isdir(dir+"textures/"):
            os.mkdir(dir+"textures/")
             
        if not path.isdir(dir+"meshes/"):
            os.mkdir(dir+"meshes/")
        if not path.isdir(dir+f"meshes/{pack}/"):
            os.mkdir(dir+f"meshes/{pack}/")
            
    def export_collection_for_pivot(self, collection, fpath):
            
        #deslect everything in the scene
        for obj in bpy.context.scene.objects:
            obj.select_set(False)
                
        #select objects from the collection
        for obj in collection.objects:
            if obj.type == 'MESH':
                obj.select_set(True)
            else:
                obj.select_set(False)
                      
        #duplicate selected objects
        for obj in bpy.context.selected_objects:
            new_obj = obj.copy()
            new_obj.data = obj.data.copy()
            bpy.context.scene.collection.objects.link(new_obj)
            obj.select_set(False)
            new_obj.select_set(True)
            
        #join every duplicate into one mesh
        if len(bpy.context.selected_objects) > 1:
            bpy.context.view_layer.objects.active = bpy.context.selected_objects[0]
            bpy.ops.object.join()
            print("joined")
        
        # split that mesh into faces by edges
        
#        fake_context = bpy.context.copy()
#        area = [area for area in bpy.context.screen.areas if area.type == "VIEW_3D"][0]
#        fake_context['area'] = area
#        fake_context['active_object'] = bpy.context.selected_objects[0]
#        bpy.ops.object.mode_set(fake_context, mode='EDIT')
        bpy.ops.object.editmode_toggle()
        bpy.ops.mesh.edge_split(type='EDGE')
        bpy.ops.object.editmode_toggle()
#        bpy.ops.object.mode_set(mode='OBJECT')
        
        
        # export it
        bpy.ops.export_scene.obj(
                filepath=fpath,
                global_scale= bpy.context.scene.scene_scale,
                use_selection=True,
                use_mesh_modifiers=True,
                use_normals=True,
                use_materials=True,
                use_triangles=True,
                use_uvs=True,  # Export UV coordinates
                axis_forward='Y',
                axis_up='Z'
            )
        
        # remove these new objects
        for new_obj in bpy.context.selected_objects:
            bpy.context.scene.collection.objects.unlink(new_obj)
            
class PivotPanel(bpy.types.Panel):
    bl_label = "Pivot Level Exporter"
    bl_idname = "PIVOT_PT_pivot_panel"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "Pivot"
    
    def draw(self, context):
        layout = self.layout
        col = layout.column()
        
        #get assests directory
        col.label(text="FILE STRUCTURE")
        row = col.row()
        row.label(text = "assets_dir")
        row.prop(context.scene, "assets_dir", text = "")
        row = col.row()
        row.label(text = "level_pack")
        row.prop(context.scene, "level_pack", text = "")
        row = col.row()
        row.label(text = "suffix")
        row.prop(context.scene, "suffix", text = "")
        col.separator(factor = 3)
        
        #get mesh collections
        col.label(text="MESHES")
        row = col.row()
        row.label(text = "Collision Mesh Collection:")
        row.prop(context.scene, "collision_meshes", text="")
        row = col.row()
        row.label(text = "Render Mesh Collection:")
        row.prop(context.scene, "decorative_meshes", text="")
        col.separator(factor = 3)
        
        #get start and end locations
        col.label(text="LOCATIONS")
        row = col.row()
        row.label(text = "Start")
        row.prop_search(context.scene, "start", bpy.data, "objects", text="")
        row = col.row()
        row.label(text = "Exit")
        row.prop_search(context.scene, "exit", bpy.data, "objects", text="")
        col.separator(factor = 3)
        
        
        #SPRITES AND LIGHTS
        col.label(text="SPRITES AND LIGHTS")
        row = col.row()
        row.label(text = "Collectible Sprite Collection:")
        row.prop(context.scene, "collectible_sprites", text="")
        row = col.row()
        row.label(text = "Decorative Sprite Collection:")
        row.prop(context.scene, "decorative_sprites", text="")
        
        #solo point lights without textures
        row = col.row()
        row.label(text = "Point Lights Collection:")
        row.prop(context.scene, "point_lights", text="")
        col.separator(factor = 3)
        
        #other settings
        col.label(text="OTHER SETTINGS")
        col.prop(context.scene, "glow_count")
        row = col.row()
        row.prop(context.scene, "scene_scale")
        col.separator(factor = 3)
        
        
        
        #make the operatory button
        col.operator("pivot.json_operator", text="Export Level JSON")
        
        
        
        
def register():
    #register file structure ui elements
    bpy.types.Scene.assets_dir = bpy.props.StringProperty(name="Assets Directory")
    bpy.types.Scene.suffix = bpy.props.IntProperty(name = "Suffix", min = 0, max = 99)
    bpy.types.Scene.level_pack = bpy.props.EnumProperty(
        name="Pack",
        description="Set the level pack",
        items=[
            ('lab', "Lab", "Lab Level"),
            ('debug', "Debug", "Debugging Level"),
            ('tutorial', "Tutorial", "Tutorial Level")
        ],
        default='debug'
    )
    
    #register mesh ui elements
    bpy.types.Scene.collision_meshes = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.decorative_meshes = bpy.props.PointerProperty(type=bpy.types.Collection)
    
    #register light elements
    bpy.types.Scene.point_lights = bpy.props.PointerProperty(type=bpy.types.Collection)
    
    #register location ui elements
    bpy.types.Scene.start = bpy.props.PointerProperty(type=bpy.types.Object)
    bpy.types.Scene.exit = bpy.props.PointerProperty(type=bpy.types.Object)
    bpy.types.Scene.collectible_sprites = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.decorative_sprites = bpy.props.PointerProperty(type=bpy.types.Collection)
    
    #register other ui elements
    bpy.types.Scene.glow_count = bpy.props.IntProperty(name = "Glowstick Count", min = 0, max = 10)
    bpy.types.Scene.scene_scale = bpy.props.IntProperty(name = "Scene Scale", min = 0, max = 10)
    
    #register the operators
    bpy.utils.register_class(ExportJsonOperator)
    bpy.utils.register_class(PivotPanel)
    
def unregister():
    del bpy.types.Scene.assets_dir
    del bpy.types.Scene.suffix
    del bpy.types.Scene.level_pack
    
    del bpy.types.Scene.collision_meshes
    del bpy.types.Scene.decorative_meshes
    
    del bpy.types.Scene.start
    del bpy.types.Scene.exit
    del bpy.types.Scene.collectible_sprites
    del bpy.types.Scene.decorative_sprites
    
    bpy.utils.unregister_class(ExportJsonOperator)
    bpy.utils.unregister_class(PivotPanel)

if __name__ == "__main__":
    register()