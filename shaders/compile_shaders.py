import os
import os.path as path

def get_shader_files():
    
    # Acceptable extensions
    extensions = ["glsl"]

    # All files in this
    files = os.listdir()

    file_names = [path.basename(f) for f in files if len(f.split('.')) == 2]
    shader_files = [f for f in file_names if f.split('.')[1] in extensions]

    return shader_files

def create_header_file(file_name : str):

    contents = ""
    with open(file_name, "r") as f:
        # Reads whole file 
        contents = f.read()

    new_contents = ",".join([hex(ord(c)) for c in contents])
    

    # Write the cc file
    h_file_name = file_name + ".h"
    with open(h_file_name, "w") as g:
        
        # Write the ifndef
        ifndef_name = h_file_name.replace('.', '_')
        g.write("#ifndef SHADER_FILE_{:s}".format(ifndef_name.upper()))
        g.write("\n#define SHADER_FILE_{:s}".format(ifndef_name.upper()))

        g.write("\nunsigned char ")
        g.write(h_file_name.split('.')[0]+"[]")
        g.write(" = {")
        g.write(new_contents)
        g.write("};")

        # Write the endif
        g.write("\n#endif \\\\ {:s}".format(ifndef_name.upper()))
        
    

def main():

    # Get all the files that we need from
    # this folder
    shader_files = get_shader_files()

    for f in shader_files:
        create_header_file(f)

if __name__ == "__main__":
    main()
