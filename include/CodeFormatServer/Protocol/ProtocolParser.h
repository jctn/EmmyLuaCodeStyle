#pragma once

#include <cstdlib>
#include <string_view>
#include <memory>
#include "CodeFormatServer/VSCode.h"

class ProtocolParser
{
public:
	ProtocolParser();

	void Parse(std::string_view msg);

	std::shared_ptr<vscode::Serializable> GetParam();

	std::string_view GetMethod();

	std::string SerializeProtocol(std::shared_ptr<vscode::Serializable> result);
private:
	int _id;
	std::string _method;
	std::shared_ptr<vscode::Serializable> _param;
};