#include "new_project.h"

namespace el {

	NewProjectDialog::NewProjectDialog(QWidget* parent) : QDialog(parent) {
		ui.setupUi(this);

		ui.projDir->setText("");
		ui.assetDir->setText("");
		ui.srcDir->setText("");
		ui.datDir->setText("");

		connect(ui.proj, &QPushButton::pressed, [&]() {
			ui.projDir->setText(QFileDialog::getExistingDirectory(parent, tr("Project Directory"), tr("D:/Programming/_Elang")) + "/");
		});
		connect(ui.asset, &QPushButton::pressed, [&]() {
			ui.assetDir->setText(QFileDialog::getExistingDirectory(parent, tr("Asset Directory"), tr("D:/Programming/_Elang")) + "/");
		});
		connect(ui.src, &QPushButton::pressed, [&]() {
			ui.srcDir->setText(QFileDialog::getExistingDirectory(parent, tr("Source Directory"), tr("D:/Programming/_Elang")) + "/");
		});
		connect(ui.dat, &QPushButton::pressed, [&]() {
			ui.datDir->setText(QFileDialog::getExistingDirectory(parent, tr("Data Directory"), tr("D:/Programming/_Elang")) + "/");
		});

		connect(ui.ok, &QPushButton::pressed, [&]() { 
			if (ui.projectName->text().isEmpty() || ui.projDir->text().isEmpty() || ui.assetDir->text().isEmpty()) {
				QMessageBox message(this);
				message.setText("Fill out the necessary components.");
				message.exec();
			} else {
				accepted = true;
				close();
			}
		});
		connect(ui.cancel, &QPushButton::pressed, [&]() {
			accepted = false;
			close();
		});
		
	}

	QString NewProjectDialog::saveNewProject(Project& project) {
		if (accepted) {
			bind(project);
			project.reset();
			project.name = ui.projectName->text().toStdString();
			project.assetDir = ui.assetDir->text().toStdString();
			project.srcDir = ui.srcDir->text().toStdString();
			project.datDir = ui.datDir->text().toStdString();
			project.internalBinary = ui.internalBinary->isChecked();

			//auto cam = gProject->make<Camera>(gProject->cameras, "Main Camera");
			//cam->to(vec3(0, 0, -1000));
			//gProject->make<Painter>(gProject->painters, "Sprite Painter",
			//	"basic_sprite", "texture_uv", 100000, cam, Projection::eOrtho,
			//	ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_READ_ONLY);
			//gProject->make<Painter>(gProject->painters, "UI Painter",
			//	"basic_sprite", "texture_uv", 100000, NullEntity, Projection::eOrtho,
			//	ePainterFlags::DEPTH_SORT | ePainterFlags::MULTI_MATERIAL | ePainterFlags::Z_CLEAR);

			QString path = ui.projDir->text() + tr("/") + ui.projectName->text() + tr(".elang");
			return path;
		} return QString();
	}
}
