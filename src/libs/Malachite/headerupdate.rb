
require 'pathname'

Qualifier = "//ExportName:"

scriptPath = Pathname.new(File.expand_path(File.dirname(__FILE__)))

srcPath = scriptPath + "src"
outputPath = scriptPath + "include/Malachite"

def exportedClasseNames(filePath)

	classnames = Array.new
	
	file = File.open(filePath, "r")
	
	while rawline = file.gets
		
		line = rawline.encode("UTF-8", "UTF-8", invalid: :replace, undef: :replace, replace: '.')
		
		tokens = line.split
		
		if tokens.length >= 2
			
			if tokens[0] == Qualifier
				classnames.push tokens[1]
			end
			
		end
	end
	
	return classnames
	
end

def writeHeaderFile(filepath, includePath)
	
	file = File.open(filepath, "w")
	file.puts('#include "' + includePath + '"')
	file.close
	
end


srcdir = Dir::open(srcPath.to_s)

srcdir.each do |file|
	
	if /\.h$/ =~ file
		
		path = srcPath + file
		
		classnames = exportedClasseNames(path.to_s)
		
		classnames.each do |name|
			
			outHeaderPath = outputPath + name
			
			writeHeaderFile(outHeaderPath.to_s, path.relative_path_from(outputPath).to_s)
			
			puts name
		end
	end
end


