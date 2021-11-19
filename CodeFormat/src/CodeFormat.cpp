#include <iostream>
#include <cstring>
#include <fstream>
#include "LuaParser/LuaParser.h"
#include "Util/format.h"
#include "CodeService/LuaFormatter.h"
#include "Util/CommandLine.h"

// https://stackoverflow.com/questions/1598985/c-read-binary-stdin
#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
# define SET_BINARY_MODE() _setmode(_fileno(stdin), _O_BINARY);\
	_setmode(_fileno(stdout), _O_BINARY)
#else
# define SET_BINARY_MODE() ((void)0)
#endif


int write_string_to_file_replace(const std::string & file_string, const std::string str )
{
	std::ofstream	OsWrite(file_string);
	OsWrite<<str;
	OsWrite<<std::endl;
	OsWrite.close();
   return 0;
}


int main(int argc, char** argv)
{
	CommandLine cmd;

	cmd.AddTarget("format");
	cmd.AddTarget("diagnosis");

	cmd.Add<std::string>("file", "f", "Specify the input file");
	cmd.Add<int>("stdin", "i", "Read from stdin and specify read size");
	cmd.Add<std::string>("config", "c",
	                     "Specify editorconfig file, it decides on the effect of formatting or diagnosis");

	if (!cmd.Parse(argc, argv))
	{
		cmd.PrintUsage();
		return -1;
	}

	std::shared_ptr<LuaParser> parser = nullptr;

	if (!cmd.Get<std::string>("file").empty())
	{
		parser = LuaParser::LoadFromFile(argv[2]);
	}
	else if (cmd.Get<int>("stdin") != 0)
	{
		SET_BINARY_MODE();
		std::size_t size = cmd.Get<int>("stdin");

		std::string buffer;
		buffer.resize(size);
		std::cin.get(buffer.data(), size, EOF);
		auto realSize = strnlen(buffer.data(), size);
		buffer.resize(realSize);
		parser = LuaParser::LoadFromBuffer(std::move(buffer));
	}

	std::shared_ptr<LuaCodeStyleOptions> options;
	if (!cmd.Get<std::string>("config").empty())
	{
		options = LuaCodeStyleOptions::ParseFromEditorConfig(cmd.Get<std::string>("config"));
	}

	parser->BuildAstWithComment();

	LuaFormatter formatter(parser, *options);
	formatter.BuildFormattedElement();

	if (cmd.GetTarget() == "format")
	{
		auto formattedText = formatter.GetFormattedText();
		//格式化写入覆盖文件
		write_string_to_file_replace(argv[2],formattedText);
		// std::cout.write(formattedText.data(), formattedText.size());
	}
	else if (cmd.GetTarget() == "diagnosis")
	{
		auto diagnosis = formatter.GetDiagnosisInfos();
		for (auto& d : diagnosis)
		{
			std::cout << format("{} from {}:{} to {}:{}", d.Message, d.Range.Start.Line, d.Range.Start.Character,
			                    d.Range.End.Line, d.Range.End.Character) << std::endl;
		}
	}
	return 0;
}
