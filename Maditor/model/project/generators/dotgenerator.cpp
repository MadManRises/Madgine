#include "maditormodellib.h"

#include "dotgenerator.h"
#include "commandline.h"

namespace Maditor {
	namespace Model {
		namespace Generators {

DotGenerator::DotGenerator(const QString & folder, const QString & name) :
	Generator(false),
	mFolder(folder),
	mName(name)
{
}

void DotGenerator::addEdge(const QString & from, const QString & to)
{
	mEdges.emplace_back(from, to);
}

void DotGenerator::addNode(const QString & node)
{
	mNodes << node;
}

void DotGenerator::generate()
{
	Generator::generate();
	QString cmd = QString("dot -Tpng -o \"%1.png\" \"%1.dot\"").arg(mFolder + mName);
	CommandLine::exec(cmd.toStdString().c_str());
}

QStringList DotGenerator::filePaths()
{
	return{mFolder + mName + ".dot"};
}

void DotGenerator::write(QTextStream & stream, int index)
{
	stream << QString("digraph %1 {\n").arg(mName);
	for (const QString &node : mNodes)
		stream << QString("%1;\n").arg(node);
	for (const std::pair<QString, QString> &edge : mEdges)
		stream << QString("%1 -> %2;\n").arg(edge.first, edge.second);
	stream << "}";
}

		}
	}
}
