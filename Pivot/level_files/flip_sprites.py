import os
from PIL import Image

def flip_sprite_grid(image_path):
    image = Image.open(image_path)

    # Get the image dimensions
    width, height = image.size

    # Calculate the number of rows and columns
    num_cols = 6
    num_rows = 6

    # Calculate the size of each sprite
    sprite_width = width // num_cols
    sprite_height = height // num_rows

    # Create a new image for the flipped sprite grid
    flipped_image = Image.new("RGBA", (width, height))

    # Rearrange the pixels to flip the sprite grid
    for row in range(num_rows):
        for col in range(num_cols):
            sprite_x = col * sprite_width
            sprite_y = row * sprite_height
            sprite = image.crop((sprite_x, sprite_y, sprite_x + sprite_width, sprite_y + sprite_height))

            # Calculate the position of the flipped sprite in the new image
            flipped_x = (num_cols - 1 - col) * sprite_width
            flipped_y = (num_rows - 1 - row) * sprite_height

            # Paste the flipped sprite into the new image
            flipped_image.paste(sprite, (flipped_x, flipped_y))

    # Create the "flipped" subfolder if it doesn't exist
    output_directory = os.path.dirname(image_path)
    flipped_folder = os.path.join(output_directory, "flipped")
    if not os.path.exists(flipped_folder):
        os.makedirs(flipped_folder)

    # Save the flipped image in the "flipped" subfolder
    flipped_image_path = os.path.join(flipped_folder, os.path.basename(image_path))
    flipped_image.save(flipped_image_path)

def process_image_directory(directory):
    # Get the list of image files in the directory
    image_files = [filename for filename in os.listdir(directory) if filename.endswith(('.png', '.jpg', '.jpeg'))]

    # Flip the sprite grid for each image
    for image_file in image_files:
        image_path = os.path.join(directory, image_file)
        flip_sprite_grid(image_path)

# Directory path containing the images
image_directory = r"C:\Users\LabUser\Documents\Luke Kratsios\Game Design\other sprites\SPRITES"

# Process the images in the directory
process_image_directory(image_directory)
