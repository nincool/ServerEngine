#include "StructDefine.h"
#include <iostream>
#include "RapidXML/rapidxml.hpp"

std::string convert_string(rapidxml::xml_attribute<char>* attr)
{
	return attr == nullptr || attr->value() == nullptr ? "" : attr->value();
}

void print_error(std::ostringstream str)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
	std::cout << str.str() << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void print_error(std::string str)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
	std::cout << str << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void print_warning(std::ostringstream str)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	std::cout << str.str() << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void print_warning(std::string str)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN);
	std::cout << str << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

StructDefine::StructDefine()
{

}

StructDefine::~StructDefine()
{

}

bool StructDefine::ReadFile(std::string& strPath, std::string& strContent)
{
	FILE* fp = fopen(strPath.c_str(), "rb");
	if (!fp)
	{
		return false;
	}

	fseek(fp, 0, SEEK_END);
	const long filelength = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	strContent.resize(filelength);
	fread((void*)strContent.data(), filelength, 1, fp);
	fclose(fp);

	return true;
}

bool StructDefine::ParseClass(std::string filepath, std::string strPath)
{
	strFilePath = filepath;

	std::cout << "--------------------------begin--------------------------" << std::endl;
	std::cout << "parse file " << strPath << std::endl;
	std::cout << "---------------------------------------------------------" << std::endl;

	std::string strContent = "";
	if (!ReadFile(strFilePath + strPath, strContent))
	{
		return false;
	}

	rapidxml::xml_document<> xDoc;
	xDoc.parse<0>((char*)strContent.c_str());
	rapidxml::xml_node<>* root = xDoc.first_node();

	if (root == nullptr)
	{
		print_error(std::ostringstream() << "file empty " << strPath);
		return false;
	}

	for (rapidxml::xml_node<>* node = root->first_node(); node; node = node->next_sibling())
	{
		NF_SHARE_PTR<ClassData> pData(new ClassData());
		vecClassData.push_back(pData);
		if (!LoadClass(node, pData))
		{
			return false;
		}
	}

	std::cout << "---------------------------end---------------------------" << std::endl;

	return true;
}

bool StructDefine::LoadClass(rapidxml::xml_node<>* node, NF_SHARE_PTR<ClassData> pData)
{
	if (node == nullptr || pData == nullptr)
	{
		return false;
	}

	pData->name = convert_string(node->first_attribute("Id"));
	if (pData->name.empty())
	{
		print_error("Id not find");
		return false;
	}
	std::cout << std::endl << "begin load class:" << pData->name << " parent:" << (pData->parent == nullptr ? "" : pData->parent->name) <<std::endl;

	pData->type = convert_string(node->first_attribute("Type"));
	if (pData->type.empty())
	{
		pData->type = pData->parent != nullptr ? pData->parent->type : "";
		if (pData->type.empty())
		{
			print_error("Type not find");
			return false;
		}
	}
	pData->desc = convert_string(node->first_attribute("Desc"));
	std::string strPath = convert_string(node->first_attribute("Path"));
	if (!strPath.empty())
	{
		if (!ParseData(strPath, *pData))
		{
			return false;
		}
	}

	std::cout << "end load class:" << pData->name << std::endl;

	// 加载子类
	for (rapidxml::xml_node<>* childNode = node->first_node(); childNode; childNode = childNode->next_sibling())
	{
		NF_SHARE_PTR<ClassData> pChild(new ClassData());
		pChild->parent = pData;
		pData->vecChild.push_back(pChild);
		if (!LoadClass(childNode, pChild))
		{
			return false;
		}
	}

	return true;
}

bool StructDefine::ParseData(std::string strPath, ClassData& data)
{
	std::cout << "begin parse data " << strPath << std::endl;
	
	data.vecIncludes.push_back(strPath);

	std::string strContent = "";
	if (!ReadFile(strFilePath + strPath, strContent))
	{
		return false;
	}

	rapidxml::xml_document<> xDoc;
	xDoc.parse<0>((char*)strContent.c_str());
	rapidxml::xml_node<>* root = xDoc.first_node();

	if (root == nullptr)
	{
		print_error(std::ostringstream() << "file empty " << strPath);
		return false;
	}

	rapidxml::xml_node<>* node = root->first_node("Propertys");
	if (node != nullptr)
	{
		if (!LoadPropertys(node, data))
		{
			return false;
		}
	}

	node = root->first_node("Records");
	if (node != nullptr)
	{
		if (!LoadRecords(node, data))
		{
			return false;
		}
	}

	std::cout << "end parse data " << strPath << std::endl;

	node = root->first_node("Includes");
	if (node != nullptr)
	{
		for (rapidxml::xml_node<>* includeNode = node->first_node(); includeNode; includeNode = includeNode->next_sibling())
		{
			std::string includePath = convert_string(includeNode->first_attribute("Path"));
			if (includePath.empty())
			{
				print_error("Include path empty");
				return false;
			}

			std::string strParent = "";
			if (data.FindInclude(includePath, strParent))
			{
				print_error(std::ostringstream() << "Include path exist " << includePath << " class:" << strParent);
				return false;
			}

			if (!ParseData(includePath, data))
			{
				return false;
			}
		}
	}

	return true;
}

bool StructDefine::LoadPropertys(rapidxml::xml_node<>* node, ClassData& data)
{
	//std::cout << "load propertys begin" << std::endl;

	// 读取属性
	for (rapidxml::xml_node<>* propNode = node->first_node(); propNode; propNode = propNode->next_sibling())
	{
		Property prop;

		prop.name = convert_string(propNode->first_attribute("Id"));
		if (prop.name.empty())
		{
			print_error(std::ostringstream() << "Property Id not find class:" << data.name);
			return false;
		}
		//std::cout << "load property name:" << prop.name << std::endl;

		prop.type = convert_string(propNode->first_attribute("Type"));
		if (prop.type.empty())
		{
			print_error(std::ostringstream() << "Property Type not find");
			return false;
		}
		prop.bPrivate = convert_string(propNode->first_attribute("Private")) == "1";
		prop.bPublic = convert_string(propNode->first_attribute("Public")) == "1";
		prop.bSave = convert_string(propNode->first_attribute("Save")) == "1";
		prop.desc = convert_string(propNode->first_attribute("Desc"));

		std::string strParent = "";
		if (data.FindProperty(prop, strParent))
		{
			print_error(std::ostringstream() << "Property already exist name:" << prop.name << " class:" << strParent << " over it");
		}

		auto it = data.mapProp.find(prop.name);
		if (it != data.mapProp.end())
		{
			print_error(std::ostringstream() << "Property already exist");
			return false;
		}

		data.mapProp[prop.name] = prop;
	}

	//std::cout << "load propertys end" << std::endl;

	return true;
}

bool StructDefine::LoadRecords(rapidxml::xml_node<>* node, ClassData& data)
{
	//std::cout << "load records begin" << std::endl;

	// 读取表
	for (rapidxml::xml_node<>* recordNode = node->first_node(); recordNode; recordNode = recordNode->next_sibling())
	{
		Record record;

		record.name = convert_string(recordNode->first_attribute("Id"));
		if (record.name.empty())
		{
			print_error(std::ostringstream() << "Record Id not find class:" << data.name );
			return false;
		}
		//std::cout << "load record name:" << record.name << std::endl;

		record.col = convert_string(recordNode->first_attribute("Col"));
		if (data.type.empty())
		{
			print_error(std::ostringstream() << "Col not find");
			return false;
		}

		record.row = convert_string(recordNode->first_attribute("Row"));
		if (data.type.empty())
		{
			print_error(std::ostringstream() << "Row not find");
			return false;
		}
		record.bPrivate = convert_string(recordNode->first_attribute("Private")) == "1";
		record.bPublic = convert_string(recordNode->first_attribute("Public")) == "1";
		record.desc = convert_string(recordNode->first_attribute("Desc"));

		std::string strParent = "";
		if (data.FindRecord(record, strParent))
		{
			print_error(std::ostringstream() << "Record already exist name:" << record.name << " class:" << strParent << " over it");
		}

		int i = 0;
		for (rapidxml::xml_node<>* colNode = recordNode->first_node(); colNode; colNode = colNode->next_sibling(), ++i)
		{
			ColData col;
			col.index = i;
			col.type = convert_string(colNode->first_attribute("Type"));
			if (col.type.empty())
			{
				print_error(std::ostringstream() << "Type not find Col:" << col.index);
				return false;
			}

			col.tag = convert_string(colNode->first_attribute("Tag"));
			if (col.tag.empty())
			{
				print_error(std::ostringstream() << "Tag not find Col:" << col.index);
				return false;
			}

			col.desc = convert_string(colNode->first_attribute("Desc"));
	
			for (auto colIt : record.vecCol)
			{
				if (colIt.tag == col.tag)
				{
					print_error(std::ostringstream() << "Tag already exist col:" << col.tag << " record:" << record.name);
					return false;
				}
			}

			record.vecCol.push_back(col);
		}

		auto it = data.mapRecord.find(record.name);
		if (it != data.mapRecord.end())
		{
			print_error(std::ostringstream() << "record already exist");
			return false;
		}

		if (::lexical_cast<int>(record.col) != record.vecCol.size())
		{
			print_error("record Col not match");
			return false;
		}

		data.mapRecord[record.name] = record;
	}

	//std::cout << "load records end" << std::endl;

	return true;
}


bool StructDefine::Save(std::string nameFile, std::string nameSpace)
{
	strSaveName = nameFile;
	strNameSpace = nameSpace;

	SaveCpp();
	SaveCs();

	return true;
}


void StructDefine::SaveCpp()
{
	std::string strCpp = strSaveName + ".hpp";
	std::string strDefine = std::string("__") + strSaveName + "_HPP__";
	FILE* hppWriter = fopen(strCpp.c_str(), "w+");

	std::string strFileHead;
	strFileHead = strFileHead
		+ "// -------------------------------------------------------------------------\n"
		+ "//    @FileName         :    " + strCpp + "\n"
		+ "//    @Author           :    StructDefine Tool\n"
		+ "//    @Module           :    " + strSaveName + "\n"
		+ "// -------------------------------------------------------------------------\n\n"
		+ "#ifndef " + strDefine + "\n"
		+ "#define " + strDefine + "\n\n"
		+ "#include <string>\n\n"
		+ "namespace " + strNameSpace + "\n{";

	fwrite(strFileHead.c_str(), strFileHead.length(), 1, hppWriter);
	
	for (auto it : vecClassData)
	{
		SaveClassCpp(hppWriter, *it);
	}

	std::string strFileEnd = "\n}\n\n#endif // " + strDefine;
	fwrite(strFileEnd.c_str(), strFileEnd.length(), 1, hppWriter);

	fclose(hppWriter);
}

void StructDefine::SaveClassCpp(FILE* hppWriter, const ClassData& data)
{
	std::string strBody = "";
	strBody = strBody
		+ "\n\n\tclass " + data.name + (data.parent != nullptr ? (std::string(" : public ") + data.parent->name) : "")
		+ "\n\t{"
		+ "\n\tpublic:"
		+ "\n\t\t// class name"
		+ "\n\t\tstatic const std::string& ThisName(){ static std::string x = \"" + data.name + "\"; return x; };";

	// 属性
	strBody += "\n\n\t\t// property";
	for (auto it : data.mapProp)
	{
		strBody += "\n\t\t// " + it.second.type + " " + it.second.desc;
		strBody += "\n\t\tstatic const std::string& " + it.second.name + "(){ static std::string x = \"" + it.second.name + "\"; return x; };";
	}

	// 表
	strBody += "\n\n\t\t// record";
	for (auto it : data.mapRecord)
	{
		if (!it.second.desc.empty())
		{
			strBody += "\n\t\t// " + it.second.desc;
		}

		strBody = strBody
			+ "\n\t\tclass " + it.second.name
			+ "\n\t\t{"
			+ "\n\t\tpublic:"
			+ "\n\t\t\t// record name"
			+ "\n\t\t\tstatic const std::string& ThisName(){ static std::string x = \"" + it.second.name + "\"; return x; };"
			+ "\n\t\t\tstatic const int Col = " + ::lexical_cast<std::string>(it.second.col) + ";"
			+ "\n\t\t\tstatic const int Row = " + ::lexical_cast<std::string>(it.second.row) + ";";
		for (int i = 0; i < it.second.vecCol.size(); ++i)
		{
			strBody += "\n\t\t\t// " + it.second.vecCol[i].type + " " + it.second.vecCol[i].desc;
			strBody += "\n\t\t\tstatic const int " + it.second.vecCol[i].tag + " = " + ::lexical_cast<std::string>(i) + ";";
		}
		strBody += "\n\t\t};";
	}
	strBody += "\n\t};";

	fwrite(strBody.c_str(), strBody.length(), 1, hppWriter);

	for (auto it : data.vecChild)
	{
		SaveClassCpp(hppWriter, *it);
	}
}


void StructDefine::SaveCs()
{
	std::string strCs = strSaveName + ".cs";
	FILE* hppWriter = fopen(strCs.c_str(), "w+");

	std::string strFileHead;
	strFileHead = strFileHead
		+ "// -------------------------------------------------------------------------\n"
		+ "//    @FileName         :    " + strCs + "\n"
		+ "//    @Author           :    StructDefine Tool\n"
		+ "//    @Module           :    " + strSaveName + "\n"
		+ "// -------------------------------------------------------------------------\n\n"
		+ "using System;\n"
		+ "using System.Collections.Generic;\n"
		+ "using System.Linq;\n"
		+ "using System.Text;\n"
		+ "using System.Threading;\n\n"
		+ "namespace " + strNameSpace + "\n{";

	fwrite(strFileHead.c_str(), strFileHead.length(), 1, hppWriter);

	for (auto it : vecClassData)
	{
		SaveClassCs(hppWriter, *it);
	}

	std::string strFileEnd = "\n}";
	fwrite(strFileEnd.c_str(), strFileEnd.length(), 1, hppWriter);

	fclose(hppWriter);
}

void StructDefine::SaveClassCs(FILE* hppWriter, const ClassData& data)
{
	std::string strBody = "";
	strBody = strBody
		+ "\n\n\tpublic class " + data.name + (data.parent != nullptr ? (std::string(" : ") + data.parent->name) : "")
		+ "\n\t{"
		+ "\n\t\t// class name"
		+ "\n\t\tpublic static readonly String ThisName = \"" + data.name + "\";";

	// 属性
	strBody += "\n\n\t\t// property";
	for (auto it : data.mapProp)
	{
		if (!it.second.bPrivate && !it.second.bPublic)
		{
			continue;
		}
		strBody += "\n\t\t// " + it.second.type + " " + it.second.desc;
		strBody += "\n\t\tpublic static readonly String " + it.second.name + " = \"" + it.second.name + "\";";
	}

	// 表
	strBody += "\n\n\t\t// record";
	for (auto it : data.mapRecord)
	{
		if (!it.second.bPrivate && !it.second.bPublic)
		{
			continue;
		}

		if (!it.second.desc.empty())
		{
			strBody += "\n\t\t// " + it.second.desc;
		}

		strBody = strBody
			+ "\n\t\tpublic class " + it.second.name
			+ "\n\t\t{"
			+ "\n\t\t\t// record name"
			+ "\n\t\t\tpublic static readonly String ThisName = \"" + it.second.name + "\";"
			+ "\n\t\t\tpublic static readonly int Col = " + ::lexical_cast<std::string>(it.second.col) + ";"
			+ "\n\t\t\tpublic static readonly int Row = " + ::lexical_cast<std::string>(it.second.row) + ";";
		for (int i = 0; i < it.second.vecCol.size(); ++i)
		{
			strBody += "\n\t\t\t// " + it.second.vecCol[i].type + " " + it.second.vecCol[i].desc;
			strBody += "\n\t\t\tpublic static readonly int " + it.second.vecCol[i].tag + " = " + ::lexical_cast<std::string>(i) + ";";
		}
		strBody += "\n\t\t};";
	}
	strBody += "\n\t};";

	fwrite(strBody.c_str(), strBody.length(), 1, hppWriter);

	for (auto it : data.vecChild)
	{
		SaveClassCs(hppWriter, *it);
	}
}