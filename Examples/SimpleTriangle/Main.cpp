#include <QApplication>
#include <QWindow>

#include <iostream>
#include <string>

#include <ZittelmenEngine.h>

std::unique_ptr<ZittelmenEngine> engine = std::make_unique<ZittelmenEngine>();

void timer_tick()
{
	engine->renderFrame();
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	

    QWindow w;
	w.setWidth(1280);
	w.setHeight(720);

	engine->setTargetRenderSurface(&w);
	engine->initialize();

	QTimer renderTimer(&w);
	//QObject::connect(renderTimer, &QTimer::timeout, w, &timer_tick);

	//renderTimer.start(0);

    w.show();

    return a.exec();
}