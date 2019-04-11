#include <QApplication>
#include <QWindow>
#include <QTimer>
#include <QObject>

#include <iostream>
#include <string>

#include <ZittelmenEngine.h>

class SimpleTriangle : public QObject {
public:
	SimpleTriangle() {
		this->w.setWidth(1280);
		this->w.setHeight(720);

		this->zi = std::make_shared<ZittelmenEngine>();
		this->zi->setTargetRenderSurface(&this->renderTarget);
		this->zi->initialize();
		this->zi->renderFrame();
		
		this->w.show();

		QObject::connect(&this->renderTimer, &QTimer::timeout, this, &SimpleTriangle::timer_tick);
		this->renderTimer.start(0);
	}
	~SimpleTriangle() {
		this->renderTimer.stop();
	}

private:
	void timer_tick()
	{
		this->zi->renderFrame();
	}

private:
	QWindow w;
	QWidget renderTarget;
	QTimer renderTimer;
	std::shared_ptr<ZittelmenEngine> zi;
	


};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	SimpleTriangle st;

	return a.exec();
}