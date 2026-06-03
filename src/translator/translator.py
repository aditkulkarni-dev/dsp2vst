import os
import re

def extract_class_name(effect_header_path):
    """
    Parses the custom effect header to find the class name.
    Looks for the pattern 'class ClassName' inside the file.
    """
    if not os.path.exists(effect_header_path):
        raise FileNotFoundError(f"Source effect header not found at: {effect_header_path}")
        
    with open(effect_header_path, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()
        
    # Regex to find 'class NAME'
    match = re.search(r'class\s+([A-Za-z0-9_]+)', content)
    if match:
        return match.group(1)
    else:
        raise ValueError("Could not find a valid C++ class definition in the header.")

def generate_juce_project(effect_header_path, template_dir, placeholder="NewProject"):

    try:
        effect_class_name = extract_class_name(effect_header_path)
        print(f"Detected Effect Class Name: {effect_class_name}")
    

        parent_dir = (os.path.dirname(os.path.abspath(template_dir)))

        output_dir = os.path.join(parent_dir, effect_class_name)
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
            print(f"Created directory: {output_dir}")
        else:
            print(f"Directory {output_dir} already exists. Overwriting files...")
        
        template_files = [
                "PluginProcessor.h",
                "PluginProcessor.cpp",
                "PluginEditor.h",
                "PluginEditor.cpp"
            ]
        
        for filename in template_files:
            src_file_path = os.path.join(template_dir, filename)
            
            if not os.path.exists(src_file_path):
                print(src_file_path)
                print(f"Warning: Template file {filename} missing from {template_dir}. Skipping.")
                continue
                
            # Read template contents
            with open(src_file_path, 'r', encoding='utf-8') as f:
                file_content = f.read()
            
            # Replace placeholder with the new class name
            translated_content = file_content.replace(placeholder, effect_class_name)
            
            # Define target output path
            dest_file_path = os.path.join(output_dir, filename)
            
            # Write the translated file out
            with open(dest_file_path, 'w', encoding='utf-8') as f:
                f.write(translated_content)
                
            print(f"  -> Generated: {dest_file_path}")
                
        print("\nSuccess! Minimal boilerplate translation complete.")
    except Exception as e:
        print(f"Error occured : {e}")

if __name__ == "__main__":
    SRC_EFFECT_HEADER = "../effects/ffDelay.h"
    JUCE_TEMPLATE_DIR = "../juce"
    PLACEHOLDER_STRING = "NewProject"
    generate_juce_project(SRC_EFFECT_HEADER, JUCE_TEMPLATE_DIR)
