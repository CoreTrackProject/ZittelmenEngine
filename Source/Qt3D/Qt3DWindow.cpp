#include "Qt3DWindow.h"

Qt3DWindow::Qt3DWindow()
{
    //this->view = new Qt3DExtras::Qt3DWindow();
    this->setTitle(QStringLiteral("3D Text CPP"));
    this->defaultFrameGraph()->setClearColor(QColor(255, 255, 255));

    auto *root = new Qt3DCore::QEntity();
	
	
	// plane
    {
        auto *plane = new Qt3DCore::QEntity(root);
        auto *planeMesh = new Qt3DExtras::QPlaneMesh();
        auto *planeTransform = new Qt3DCore::QTransform();
        auto *planeMaterial = new Qt3DExtras::QPhongMaterial(root);
        planeMesh->setWidth(100); 
		planeMesh->setHeight(100);
        planeTransform->setTranslation(QVector3D(0, 0, 0));
        planeMaterial->setDiffuse(QColor(150, 150, 150));

        plane->addComponent(planeMaterial);
        plane->addComponent(planeMesh);
        plane->addComponent(planeTransform);
    }

    //auto *textMaterial = new Qt3DExtras::QPhongMaterial(root);
    //{ // text
    //    int i = 0;
    //    const QStringList fonts = QFontDatabase().families();

    //    for (const QString &family : fonts)
    //    {
    //        auto *text = new Qt3DCore::QEntity(root);
    //        auto *textMesh = new Qt3DExtras::QExtrudedTextMesh();

    //        auto *textTransform = new Qt3DCore::QTransform();
    //        QFont font(family, 32, -1, false);
    //        textTransform->setTranslation(QVector3D(-2.45f, i * .5f, 0));
    //        textTransform->setScale(.2f);
    //        textMesh->setDepth(.45f);
    //        textMesh->setFont(font);
    //        textMesh->setText(family);
    //        textMaterial->setDiffuse(QColor(111, 150, 255));

    //        text->addComponent(textMaterial);
    //        text->addComponent(textMesh);
    //        text->addComponent(textTransform);

    //        i++;
    //    }
    //}

	// camera
    { 
        float aspect = static_cast<float>(this->screen()->size().width()) / this->screen()->size().height();
        Qt3DRender::QCamera *camera = this->camera();
		//camera->lens()->setOrthographicProjection(1000.0f, 1000.0f, 1000.0f, 1000.0f, 1000.0f, 1000.0f);
        camera->lens()->setPerspectiveProjection(65.f, aspect, 0.1f, 100.f);
        camera->setPosition(QVector3D(0, 5, 3));
        camera->setViewCenter(QVector3D(0, 0, 0));

        auto *cameraController = new Qt3DExtras::QFirstPersonCameraController(root);

		cameraController->setCamera(camera);
    }

    this->setRootEntity(root);
}
