import bpy
import json
import os
from os import path
from mathutils import Vector



class ExportJsonOperator(bpy.types.Operator):
    bl_idname = "pivot.json_operator"
    bl_label = "Pivot Json Operator"
    
    def execute(self, context):
        
        #scene scale from other settings
        scene_scale = bpy.context.scene.scene_scale
        
        #deslect everything  
        for object in bpy.data.objects:
            object.select_set(False)        
    
        
        # make sure the choosen assets folder exists
        dir = bpy.context.scene.assets_dir
        if not path.isdir(dir):
            raise Exception("the provided assets dir does not exist")
        else:
            print("assets folder successfully found")
        
        
        # get pack name
        pack = bpy.context.scene.level_pack    
        self.checkFileStructure(dir, pack)
        
        #init a dict to become the json
        json_dict = dict()
        
        #init a dictionary of data which will update assests.json
        asset_updates_dict = dict()
        asset_updates_dict["jsons"] = dict()
        asset_updates_dict["textures"] = dict()
        
        #Set name to pack plus suffix
        name = pack+ "_" + str(bpy.context.scene.suffix).zfill(4)
        
        # enter level id to json
        json_dict["level_id"] = name 
        
        # enter start and end data into json
        json_dict.update(self.start_end_to_dict(scene_scale))
        
        
        # enter glowstick count 
        json_dict["glowsticks"] = bpy.context.scene.glow_count
    
        
        # enter collectibles    
        collectibles = bpy.context.scene.collectible_sprites  
        col_dict, new_tex_dict, sprite_count = self.collectibles_to_dict(
            collectibles, 
            0, 
            scene_scale, 
            dir, 
            False
        )
        json_dict["sprites"] = col_dict
        asset_updates_dict["textures"].update(new_tex_dict)
        
        # enter emissive collectibles    
        e_collectibles = bpy.context.scene.e_collectible_sprites  
        e_col_dict, new_tex_dict, sprite_count = self.collectibles_to_dict(
            e_collectibles, 
            sprite_count, 
            scene_scale, 
            dir, 
            True
        )
        json_dict["sprites"] = e_col_dict
        asset_updates_dict["textures"].update(new_tex_dict)
        
        
        # enter decorations
        decorations = bpy.context.scene.decorative_sprites
        dec_dict, new_tex_dict, sprite_count = self.dec_to_dict(
            decorations, 
            sprite_count, 
            scene_scale, 
            dir, 
            False
        )
        json_dict["sprites"].update(dec_dict)
        asset_updates_dict["textures"].update(new_tex_dict)
        
        # enter emissive decorations
        e_decorations = bpy.context.scene.e_decorative_sprites
        e_dec_dict, new_tex_dict, sprite_count = self.dec_to_dict(
            e_decorations, 
            sprite_count, 
            scene_scale, 
            dir, 
            True
        )
        json_dict["sprites"].update(e_dec_dict)
        asset_updates_dict["textures"].update(new_tex_dict)
        
        
        # enter posters
        posters = bpy.context.scene.poster_sprites
        post_dict, new_tex_dict, sprite_count = self.posters_to_dict(
            posters, 
            sprite_count,
            scene_scale, 
            dir, 
            False
        )
        json_dict["sprites"].update(post_dict)
        asset_updates_dict["textures"].update(new_tex_dict)
        
        # enter emissive posters
        e_posters = bpy.context.scene.e_poster_sprites
        e_post_dict, new_tex_dict, sprite_count = self.posters_to_dict(
            e_posters, 
            sprite_count,
            scene_scale, 
            dir, 
            True
        )
        json_dict["sprites"].update(e_post_dict)
        asset_updates_dict["textures"].update(new_tex_dict)
         
         
        # enter the lights
        lights = bpy.context.scene.point_lights
        light_dict, sprite_count = self.lights_to_dict(
            lights,
            sprite_count, 
            scene_scale
        )
        json_dict["sprites"].update(light_dict)
        
        
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
        
        
        trigger_dict = dict()
        trig_count = 0
        #export the trigger meshes as an separate objs
        #death meshes
        fpath_base = dir+ f"meshes/{pack}/{name}_death_"
        collection = bpy.context.scene.death_meshes
        for i in range(len(collection.objects)):
            if collection.objects[i].type == 'MESH':
                fpath = fpath_base + str(i).zfill(4) + ".obj"
                self.export_mesh_for_pivot(collection.objects[i], fpath)
                trigger_dict[str(trig_count)] = dict()
                trigger_dict[str(trig_count)]["type"] = "DEATH"
                trigger_dict[str(trig_count)]["mesh"] = f"meshes/{pack}/{name}_death_" + str(i).zfill(4) + ".obj"
                trigger_dict[str(trig_count)]["image"] = None
                trigger_dict[str(trig_count)]["message"] = None
                trig_count += 1
                        
        
        #popup meshes
        fpath_base = dir+ f"meshes/{pack}/{name}_popup_"
        collection = bpy.context.scene.popup_meshes
        for i in range(len(collection.objects)):
            if collection.objects[i].type == 'MESH':
                fpath = fpath_base + str(i).zfill(4) + ".obj"
                self.export_mesh_for_pivot(collection.objects[i], fpath)                   
                
                trigger_dict[str(trig_count)] = dict()
                trigger_dict[str(trig_count)]["type"] = "POPUP"
                trigger_dict[str(trig_count)]["mesh"] = f"meshes/{pack}/{name}_popup_" + str(i).zfill(4) + ".obj"
                
                new_tex_dict = self.texture_check(dir, collection.objects[i].active_material.name)
                asset_updates_dict["textures"].update(new_tex_dict)
                
                trigger_dict[str(trig_count)]["image"] = collection.objects[i].active_material.name
                trigger_dict[str(trig_count)]["message"] = None
                trig_count += 1
                
        #message meshes
        fpath_base = dir+ f"meshes/{pack}/{name}_message_"
        collection = bpy.context.scene.message_meshes
        for i in range(len(collection.objects)):
            if collection.objects[i].type == 'MESH':
                fpath = fpath_base + str(i).zfill(4) + ".obj"
                self.export_mesh_for_pivot(collection.objects[i], fpath)                   
                
                trigger_dict[str(trig_count)] = dict()
                trigger_dict[str(trig_count)]["type"] = "MESSAGE"
                trigger_dict[str(trig_count)]["mesh"] = f"meshes/{pack}/{name}_message_" + str(i).zfill(4) + ".obj"
                trigger_dict[str(trig_count)]["image"] = None
                trigger_dict[str(trig_count)]["message"] = collection.objects[i].children[0].data.body
                trig_count += 1
                
                
        #exit region
        fpath_base = dir+ f"meshes/{pack}/{name}_exitregion_"
        collection = bpy.context.scene.exit_region
        for i in range(len(collection.objects)):
            if collection.objects[i].type == 'MESH':
                fpath = fpath_base + str(i).zfill(4) + ".obj"
                self.export_mesh_for_pivot(collection.objects[i], fpath)                   
                
                trigger_dict[str(trig_count)] = dict()
                trigger_dict[str(trig_count)]["type"] = "EXITREGION"
                trigger_dict[str(trig_count)]["mesh"] = f"meshes/{pack}/{name}_exitregion_" + str(i).zfill(4) + ".obj"
                trigger_dict[str(trig_count)]["image"] = None
                trigger_dict[str(trig_count)]["message"] = None
                trig_count += 1
        
        
        json_dict["triggers"] = trigger_dict
        
        #--------------END of json entries-------------------------#
        
        # write the json
        json_path = dir+f"json/level_json/{pack}/{name}.json"
        asset_updates_dict["jsons"][name] = f"json/level_json/{pack}/{name}.json"
        with open(json_path, 'w') as file:
            json.dump(json_dict, file, indent = 4)
            
        #open the assets json and update contents
        
        with open(dir+"json/assets.json", 'r') as file:
            asset_dict = json.load(file)
            asset_dict["jsons"].update(asset_updates_dict["jsons"])
            
            # only update the texture if it does not already exist
            for tex in list(asset_updates_dict["textures"]):
                print(tex)
                if tex not in asset_dict["textures"].keys():
                    print("is new")
                    asset_dict["textures"][tex] =  asset_updates_dict["textures"][tex]
        
        # replace the old assets with updated assets
        #NOTE user is responsible for deleting assets for old levels
        with open(dir+"json/assets.json", 'w') as file:
            json.dump(asset_dict,file, indent = 4)
        
        return {'FINISHED'}
    
    def texture_check(self, dir, name):
        
        print(f"checking{name}")
        
        if not path.isfile(dir+ f"textures/{name}.png"):
            raise Exception(f"{name}.png was not found in the assets/textures/ directory. Please add it.")

        #queue texture to be added to assets   
        new_tex_dict = dict() 
        new_tex_dict[f"{name}"] = {
            "file":f"textures/{name}.png",
            "span": 1,
            "cols": 1
            }
        return new_tex_dict
    
    def start_end_to_dict(self, scene_scale):
            
        #init a dict
        se_dict = dict()
        
        # get start and exit data
        start = bpy.context.scene.start
        exit = bpy.context.scene.exit
        
        if start.type != 'EMPTY': raise Exception("the start should be an empty")
        else: 
            se_dict["player_loc"] = [
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
            se_dict["norm"] = [vec[0],vec[1],vec[2]]
            
        if exit.type != 'EMPTY': raise Exception("the exit should be an empty")
        else: se_dict["exit"] = [
            exit.location.x * scene_scale,
            exit.location.y * scene_scale,
            exit.location.z * scene_scale
            ]
        return se_dict
    
    def collectibles_to_dict(self, collection, sprite_count, scene_scale, dir,emit_bool):
                
        #get sprite data
        collectibles = collection
        col_dict = dict()
        
        for sprite in collectibles.objects:
            
            if sprite.parent is None:
                
                new_tex_dict = self.texture_check(dir, sprite.active_material.name)
                
                sprite_dict = dict()
                
                # appearance properties
                sprite_dict["loc"] = [sprite.location.x * scene_scale,sprite.location.y * scene_scale,sprite.location.z * scene_scale]
                sprite_dict["tex"] = sprite.active_material.name
                sprite_dict["scale"] = sprite.scale[2]
                
                matrix_world = sprite.matrix_world
                vec = Vector((round(matrix_world[0][2]), 
                    round(matrix_world[1][2]), 
                    round(matrix_world[2][2])
                    ))
                vec.normalize()
                sprite_dict["norm"] = [vec[0],vec[1],vec[2]]
                
                #boolean Properties
                sprite_dict["collectible"] = True
                sprite_dict["billboard"] = True
                sprite_dict["emissive"] = emit_bool
                
                #light properties
                sprite_dict["color"] = None
                sprite_dict["intense"] = None
                sprite_dict["radius"] = None
                               
                
                for ch in sprite.children:
                    if ch.data.type  == 'POINT':
                        sprite_dict["color"] = [ch.data.color.r, ch.data.color.g, ch.data.color.b]
                        sprite_dict["intense"] = ch.data.energy/100000
                        sprite_dict["radius"] = ch.data.shadow_soft_size*scene_scale
                
                col_dict[f"{sprite_count}"] = sprite_dict
                sprite_count += 1
                
        return (col_dict, new_tex_dict, sprite_count)
    
    def dec_to_dict(self, collection, sprite_count, scene_scale, dir, emit_bool):
        
        decorations = collection
        deco_dict = dict()
        new_tex_dict = dict()
        
        
        for sprite in decorations.objects:
            
            if sprite.parent is None:
                new_tex_dict = self.texture_check(dir, sprite.active_material.name)
                
                sprite_dict = dict()
                
                # appearance properties
                sprite_dict["loc"] = [sprite.location.x * scene_scale,sprite.location.y * scene_scale,sprite.location.z * scene_scale]
                sprite_dict["tex"] = sprite.active_material.name
                sprite_dict["scale"] = sprite.scale[2]
                
                matrix_world = sprite.matrix_world
                vec = Vector((round(matrix_world[0][2]), 
                    round(matrix_world[1][2]), 
                    round(matrix_world[2][2])
                    ))
                vec.normalize()
                sprite_dict["norm"] = [vec[0],vec[1],vec[2]]
                
                #boolean Properties
                sprite_dict["collectible"] = False
                sprite_dict["billboard"] = True
                sprite_dict["emissive"] = emit_bool
                
                #light properties
                sprite_dict["color"] = None
                sprite_dict["intense"] = None
                sprite_dict["radius"] = None
                
                
                for ch in sprite.children:
                    if ch.data.type == 'POINT':
                        sprite_dict["color"] = [ch.data.color.r, ch.data.color.g, ch.data.color.b]
                        sprite_dict["intense"] = ch.data.energy/100000
                        sprite_dict["radius"] = ch.data.shadow_soft_size*scene_scale
                
                deco_dict[f"{sprite_count}"] = sprite_dict
                sprite_count += 1
                
        return (deco_dict, new_tex_dict, sprite_count)
    
    def lights_to_dict(self, collection, sprite_count, scene_scale):
        
        light_dict = dict()
    
        #get solo point light data and make sprites wo textures
        point_lights = collection
        
        for obj in point_lights.objects:
            if obj.data.type == 'POINT':
                
                sprite_dict = dict()
                
                # appearance properties
                sprite_dict["loc"] = [obj.location.x * scene_scale, obj.location.y * scene_scale, obj.location.z * scene_scale]
                sprite_dict["tex"] = None
                sprite_dict["scale"] = None
                sprite_dict["norm"] = None
                
                #boolean Properties
                sprite_dict["collectible"] = False
                sprite_dict["billboard"] = False
                sprite_dict["emissive"] = False 
                
                #light properties
                sprite_dict["color"] = [obj.data.color.r,obj.data.color.g,obj.data.color.b]
                sprite_dict["intense"] = obj.data.energy/100000
                sprite_dict["radius"] = obj.data.shadow_soft_size*scene_scale
                
                
                light_dict[f"{sprite_count}"] = sprite_dict
                sprite_count += 1  
        
        return light_dict, sprite_count
    
    def posters_to_dict(self, collection, sprite_count, scene_scale, dir, emit_bool):
        
        posters = collection
        poster_dict = dict()
        new_tex_dict = dict()
        
        
        for sprite in posters.objects:
            
            if sprite.parent is None:
                new_tex_dict = self.texture_check(dir, sprite.active_material.name)
                
                sprite_dict = dict()
                
                # appearance properties
                sprite_dict["loc"] = [sprite.location.x * scene_scale,sprite.location.y * scene_scale,sprite.location.z * scene_scale]
                sprite_dict["tex"] = sprite.active_material.name
                sprite_dict["scale"] = sprite.scale[2]
                
                matrix_world = sprite.matrix_world
                vec = Vector((round(matrix_world[0][2]), 
                    round(matrix_world[1][2]), 
                    round(matrix_world[2][2])
                    ))
                vec.normalize()
                sprite_dict["norm"] = [vec[0],vec[1],vec[2]]
                
                #boolean Properties
                sprite_dict["collectible"] = False
                sprite_dict["billboard"] = False
                sprite_dict["emissive"] = emit_bool
                
                #light properties
                sprite_dict["color"] = None
                sprite_dict["intense"] = None
                sprite_dict["radius"] = None
                
                
                for ch in sprite.children:
                    if ch.data.type == 'POINT':
                        sprite_dict["color"] = [ch.data.color.r, ch.data.color.g, ch.data.color.b]
                        sprite_dict["intense"] = ch.data.energy/100000
                        sprite_dict["radius"] = ch.data.shadow_soft_size*scene_scale
                
                poster_dict[f"{sprite_count}"] = sprite_dict
                sprite_count += 1
                
        return (poster_dict, new_tex_dict, sprite_count)
    
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
            
    def export_mesh_for_pivot(self, obj, fpath):
        
        obj.select_set(True)
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
        obj.select_set(False)
        
        
            
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
        
        col.label(text="Triggers")
        row = col.row()
        row.label(text = "Death Triggers:")
        row.prop(context.scene, "death_meshes", text="")
        row = col.row()
        row.label(text = "Popup Triggers:")
        row.prop(context.scene, "popup_meshes", text="")
        row = col.row()
        row.label(text = "Message Triggers:")
        row.prop(context.scene, "message_meshes", text="")
        row = col.row()
        row.label(text = "Exit Trigger:")
        row.prop(context.scene, "exit_region", text="")
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
        col.label(text="SPRITES")
        row = col.row()
        row.label(text = "Collectible Sprites:")
        row.prop(context.scene, "collectible_sprites", text="")
        row = col.row()
        row.label(text = "Decorative Sprites:")
        row.prop(context.scene, "decorative_sprites", text="")
        row = col.row()
        row.label(text = "Poster Sprites:")
        row.prop(context.scene, "poster_sprites", text="")
        col.separator(factor = 3)
        
        #emissive sprites
        col.label(text="EMISSIVE SPRITES")
        row = col.row()
        row.label(text = "e-Collectible Sprites:")
        row.prop(context.scene, "e_collectible_sprites", text="")
        row = col.row()
        row.label(text = "e-Decorative Sprites:")
        row.prop(context.scene, "e_decorative_sprites", text="")
        row = col.row()
        row.label(text = "e-Poster Sprites:")
        row.prop(context.scene, "e_poster_sprites", text="")
        col.separator(factor = 3)
        
        #solo point lights without textures
        col.label(text="LIGHTS")
        row = col.row()
        row.label(text = "Point Lights Collection:")
        row.prop(context.scene, "point_lights", text="")
        col.separator(factor = 3)
        
        #other settings
        col.label(text="OTHER SETTINGS")
        col.prop(context.scene, "glow_count")
        row = col.row()
        row.prop(context.scene, "scene_scale")
        row = col.row()
        row.prop(context.scene, "depth_shade_dist")
        row = col.row()
        row.prop(context.scene, "bg_color")
        row = col.row()
        row.prop(context.scene, "bg_image")
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
            ('debug', "Debug", "Debugging Level"),
            ('tutorial', "Tutorial", "Tutorial Level"),
            ('lab', "Lab", "Lab Level"),
            ('tunnels', "Tunnels", "Tunnel Level"),
            ('woods', "Woods", "Woods Level"),
            ('finalfacility', "Final Facility", "Final Facility Level")           
        ],
        default='debug'
    )
    
    #register mesh ui elements
    bpy.types.Scene.collision_meshes = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.decorative_meshes = bpy.props.PointerProperty(type=bpy.types.Collection)
    
    #register trigger ui elements
    bpy.types.Scene.death_meshes = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.popup_meshes = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.message_meshes = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.exit_region = bpy.props.PointerProperty(type=bpy.types.Collection)
    
    #register location ui elements
    bpy.types.Scene.start = bpy.props.PointerProperty(type=bpy.types.Object)
    bpy.types.Scene.exit = bpy.props.PointerProperty(type=bpy.types.Object)
    
    #register collections
    bpy.types.Scene.collectible_sprites = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.decorative_sprites = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.poster_sprites = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.e_collectible_sprites = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.e_decorative_sprites = bpy.props.PointerProperty(type=bpy.types.Collection)
    bpy.types.Scene.e_poster_sprites = bpy.props.PointerProperty(type=bpy.types.Collection)
    
    #register light elements
    bpy.types.Scene.point_lights = bpy.props.PointerProperty(type=bpy.types.Collection)
    
    #register other ui elements
    bpy.types.Scene.glow_count = bpy.props.IntProperty(name = "Glowstick Count", min = 0, max = 10)
    bpy.types.Scene.scene_scale = bpy.props.IntProperty(name = "Scene Scale", min = 0, max = 10)
    bpy.types.Scene.depth_shade_dist = bpy.props.IntProperty(name = "Depth_Shade_Level", min = 0, max = 10000)
    bpy.types.Scene.shade_color = bpy.props.FloatVectorProperty(
        name="Shade Color",
        subtype='COLOR',
        default=(1.0, 1.0, 1.0),
        min=0.0,
        max=1.0
    )
    bpy.types.Scene.bg_color = bpy.props.FloatVectorProperty(
        name="Background Color",
        subtype='COLOR',
        default=(1.0, 1.0, 1.0),
        min=0.0,
        max=1.0
    )
    bpy.types.Scene.bg_image = bpy.props.StringProperty(
        name="BG Image",
        subtype='FILE_PATH',
    )
    
    #register the operators
    bpy.utils.register_class(ExportJsonOperator)
    bpy.utils.register_class(PivotPanel)
    
def unregister():
    del bpy.types.Scene.assets_dir
    del bpy.types.Scene.suffix
    del bpy.types.Scene.level_pack
    
    del bpy.types.Scene.collision_meshes
    del bpy.types.Scene.decorative_meshes
    
    del bpy.types.Scene.death_meshes
    del bpy.types.Scene.popup_meshes
    del bpy.types.Scene.message_meshes
    del bpy.types.Scene.exit_region
    
    del bpy.types.Scene.start
    del bpy.types.Scene.exit
    
    del bpy.types.Scene.collectible_sprites
    del bpy.types.Scene.decorative_sprites
    del bpy.types.Scene.poster_sprites
    del bpy.types.Scene.e_collectible_sprites
    del bpy.types.Scene.e_decorative_sprites
    del bpy.types.Scene.e_poster_sprites
    
    del bpy.types.Scene.point_lights   
    
    del bpy.types.Scene.glow_count
    del bpy.types.Scene.scene_scale
    del bpy.types.Scene.depth_shade_dist
    del bpy.types.Scene.shade_color
    del bpy.types.Scene.bg_color
    del bpy.types.Scene.bg_image
    
    
    bpy.utils.unregister_class(ExportJsonOperator)
    bpy.utils.unregister_class(PivotPanel)

if __name__ == "__main__":
    register()