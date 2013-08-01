require 'pathname'

Qualifier = "//ExportName:"

script_path = Pathname.new(__FILE__).parent

src__dir_path = script_path + "src"
output_dir_path = script_path + "include/Malachite"

def get_exported_class_name(filepath)

  filepath.each_line do |line|

    tokens = line.split
    if tokens.length >= 2 && tokens[0] == Qualifier
      return tokens[1]
    end

  end

  return nil

end

def write_header_file(filepath, include_path)

  filepath.open("w") do |file|
    file.puts("#include \"#{include_path}\"")
  end

end

src__dir_path.each_child do |filepath|

  if /\.h$/ =~ filepath.to_s

    classname = get_exported_class_name(filepath)

    if classname
      out_path = output_dir_path + classname
      write_header_file(out_path, filepath.relative_path_from(output_dir_path))
      puts classname
    end

  end

end
