#include <QApplication>
#include <QWindow>
#include <QTimer>
#include <QObject>
#include <QMainWindow>
#include <QResizeEvent>
#include <QBoxLayout>

#include <iostream>
#include <string>

#include <API/ZiEngine.h>

/*

*/
class SimpleMesh : public QMainWindow {
public:
	SimpleMesh() {


		// Create the ZiEngine object
		this->zi = std::make_shared<ZiEngine>();

		// Set the SimpleTriangle window a target render surface
		this->zi->SetTargetRenderSurface(this);

		// Lets load a mesh which is hold in a ZiMesh object
		std::shared_ptr<ZiMesh> mesh = 
			std::make_shared<ZiMesh>(
				ZiMesh::GetQuadVertexCollection(), 
				ZiMesh::GetQuadVertexIndexCollection(), 
				std::make_shared<ZiTexture>("D:/coretrack_devel/texture.jpg"));

		// create a new scene which holds the mesh
		std::shared_ptr<ZiScene> scene = std::make_shared<ZiScene>();

		// Add the mesh to the scene
		scene->AddEntity(mesh);

		// Set the current scene 
		this->zi->SetScene(scene);

		// TODO: Remove initialization function call
		// Initialize the engine
		this->zi->Initialize();


		QObject::connect(&this->renderTimer, &QTimer::timeout, this, &SimpleMesh::timer_tick);
		this->renderTimer.start(0);

	}

	~SimpleMesh() {
		this->renderTimer.stop();
	}


protected:
	bool event(QEvent * event) 
	{
		if (event->type() == QEvent::Resize) {
			QResizeEvent *resizeEvent = static_cast<QResizeEvent*>(event);
			this->zi->Resize(
				resizeEvent->size().width(),
				resizeEvent->size().height()
			);
		} else if (event->type() == QEvent::Close) {
			this->zi->Destroy();
		}
		return QMainWindow::event(event);
	}

private:
	void timer_tick()
	{
		this->zi->RenderFrame();
	}


private:
	QTimer renderTimer;
	std::shared_ptr<ZiEngine> zi;
	
};

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	SimpleMesh tm;
	tm.show();

	return a.exec();
}