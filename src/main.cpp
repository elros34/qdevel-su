#include <QtQuick>
#include <sailfishapp.h>
#include "qdevelsu.h"

int main(int argc, char *argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.setApplicationDescription("Graphical wrapper for devel-su");
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);

    QCommandLineOption titleOption({"t", "title"}, "Title", "title");
    parser.addOption(titleOption);
    QCommandLineOption descriptionOption({"d", "description"}, "Description", "description");
    parser.addOption(descriptionOption);
    parser.process(*app);

    QStringList args = parser.positionalArguments();
    qDebug() << "args: " << args;

    QQmlApplicationEngine engine;
    QDevelSu qdevelSu(args, parser.value(titleOption), parser.value(descriptionOption));
    engine.rootContext()->setContextProperty("qdevelSu", &qdevelSu);
    engine.load(SailfishApp::pathToMainQml());

    return app->exec();
}
