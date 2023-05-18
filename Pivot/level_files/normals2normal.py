import os

def rename_normals(directory):
    # Get the list of files in the directory
    file_names = os.listdir(directory)

    # Rename files ending with "normals"
    for file_name in file_names:
        name, ext = os.path.splitext(file_name)
        if name.endswith("normals"):
            old_path = os.path.join(directory, file_name)
            new_name = name[:-1] + ext
            new_path = os.path.join(directory, new_name)
            os.rename(old_path, new_path)

# Directory path
directory = r"C:\Users\LabUser\Documents\Luke Kratsios\Game Design\other sprites\SPRITES\flipped"

# Rename files in the directory
rename_normals(directory)
