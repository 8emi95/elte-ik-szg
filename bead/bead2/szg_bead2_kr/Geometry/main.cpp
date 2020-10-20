#include <osg/Texture2D>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>

#ifdef _DEBUG
#pragma comment(lib, "osgd.lib")
#pragma comment(lib, "osgDBd.lib")
#pragma comment(lib, "osgViewerd.lib")
#else
#pragma comment(lib, "osg.lib")
#pragma comment(lib, "osgDB.lib")
#pragma comment(lib, "osgViewer.lib")
#endif

#pragma warning(disable : 4482 )

osg::ref_ptr<osg::Geode> GetDisk(float radius, osg::Vec3 normal) {
	// pozíciók, textúra és normálisok
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texarray = new osg::Vec2Array;
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;

	vertices->push_back(osg::Vec3(0, 0, 0)); texarray->push_back(osg::Vec2(1, 1)); normals->push_back(normal);
	int N = 16;

	for (int i = 0; i < N + 1; ++i) {
		float u = 2 * osg::PI / N * i;
		osg::Vec3 p = osg::Vec3(radius * cos(u), 0, radius * sin(u));
		vertices->push_back(p); texarray->push_back(osg::Vec2(cos(u), sin(u))); normals->push_back(normal);
	}

	osg::ref_ptr<osg::Geometry> diskGeometry = new osg::Geometry;
	diskGeometry->setUseDisplayList(false);
	diskGeometry->setUseVertexBufferObjects(true);
	diskGeometry->setVertexArray(vertices.get());
	diskGeometry->setNormalArray(normals.get());
	diskGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	diskGeometry->setTexCoordArray(0, texarray.get());
	diskGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, N + 2));
	osg::ref_ptr<osg::Geode> disk = new osg::Geode;
	disk->addDrawable(diskGeometry.get());
	return disk.get();
}

void GetCylinderLateral(float u, float v, float height, float radius,
	osg::ref_ptr<osg::Vec3Array>& vertices,
	osg::ref_ptr<osg::Vec3Array>& normals,
	osg::ref_ptr<osg::Vec2Array>& texcoords
) {
	vertices->push_back(osg::Vec3(radius * cos(2 * osg::PI * u), height * (v - 0.5f), radius * sin(2 * osg::PI * u)));
	normals->push_back(osg::Vec3(cos(2 * osg::PI * u), 0, sin(2 * osg::PI * u)));
	texcoords->push_back(osg::Vec2(u, v));
}

osg::ref_ptr<osg::Group> GetCylinder(float height, float radius) {
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

	const int N = 16;
	float delta = 1.0 / N;
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			float u = i*delta;
			float v = j*delta;
			// 1. háromszög: x,y x+delta,y y+delta,x
			GetCylinderLateral(u, v, height, radius, vertices, normals, texcoords);
			GetCylinderLateral(u, v + delta, height, radius, vertices, normals, texcoords);
			GetCylinderLateral(u + delta, v, height, radius, vertices, normals, texcoords);
			// 2. háromszög: x+delta,y x+delta,y+delta y+delta,x
			GetCylinderLateral(u + delta, v, height, radius, vertices, normals, texcoords);
			GetCylinderLateral(u, v + delta, height, radius, vertices, normals, texcoords);
			GetCylinderLateral(u + delta, v + delta, height, radius, vertices, normals, texcoords);
		}
	}
	
	//a henger palástja
	osg::ref_ptr<osg::Geometry> cylinderLateralGeometry = new osg::Geometry;
	cylinderLateralGeometry->setUseDisplayList(false);
	cylinderLateralGeometry->setUseVertexBufferObjects(true);

	cylinderLateralGeometry->setVertexArray(vertices.get());
	cylinderLateralGeometry->setNormalArray(normals.get());
	cylinderLateralGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	cylinderLateralGeometry->setTexCoordArray(0, texcoords.get());

	cylinderLateralGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 6 * N*N));

	osg::ref_ptr<osg::Geode> cylinderLateral = new osg::Geode;
	cylinderLateral->addDrawable(cylinderLateralGeometry.get());

	osg::ref_ptr<osg::MatrixTransform> bottomTransform = new osg::MatrixTransform;
	bottomTransform->setMatrix(osg::Matrix::translate(osg::Vec3(0, -height / 2.0f, 0)));
	bottomTransform->addChild(GetDisk(radius, osg::Vec3(0.0f, -1.0f, 0.0f)));
	osg::ref_ptr<osg::MatrixTransform> topTransform = new osg::MatrixTransform;
	topTransform->setMatrix(osg::Matrix::translate(osg::Vec3(0, height / 2.0f, 0)));
	topTransform->addChild(GetDisk(radius, osg::Vec3(0.0f, 1.0f, 0.0f)));

	osg::ref_ptr<osg::Group> cylinder = new osg::Group;
	cylinder->addChild(cylinderLateral);
	cylinder->addChild(bottomTransform);
	cylinder->addChild(topTransform);
	return cylinder.get();
}

void GetConeLateral(float u, float v, float height, float radius,
	osg::ref_ptr<osg::Vec3Array>& vertices,
	osg::ref_ptr<osg::Vec3Array>& normals,
	osg::ref_ptr<osg::Vec2Array>& texcoords
) {
	vertices->push_back(osg::Vec3((1 - v) * radius * cos(2 * osg::PI * u), height * (v - 0.5f), (1 - v) * radius * sin(2 * osg::PI * u)));
	normals->push_back(osg::Vec3(cos(2 * osg::PI * u), 0, sin(2 * osg::PI * u)));
	texcoords->push_back(osg::Vec2(u, v));
}

osg::ref_ptr<osg::Group> GetCone(float height, float radius) {
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

	const int N = 16;
	float delta = 1.0 / N;
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			float u = i*delta;
			float v = j*delta;
			// 1. háromszög: x,y x+delta,y y+delta,x
			GetConeLateral(u, v, height, radius, vertices, normals, texcoords);
			GetConeLateral(u, v + delta, height, radius, vertices, normals, texcoords);
			GetConeLateral(u + delta, v, height, radius, vertices, normals, texcoords);
			// 2. háromszög: x+delta,y x+delta,y+delta y+delta,x
			GetConeLateral(u + delta, v, height, radius, vertices, normals, texcoords);
			GetConeLateral(u, v + delta, height, radius, vertices, normals, texcoords);
			GetConeLateral(u + delta, v + delta, height, radius, vertices, normals, texcoords);
		}
	}

	// a kúp palástja
	osg::ref_ptr<osg::Geometry> coneLateralGeometry = new osg::Geometry;
	coneLateralGeometry->setUseDisplayList(false);
	coneLateralGeometry->setUseVertexBufferObjects(true);


	coneLateralGeometry->setVertexArray(vertices.get());
	coneLateralGeometry->setNormalArray(normals.get());
	coneLateralGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	coneLateralGeometry->setTexCoordArray(0, texcoords.get());

	coneLateralGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 6 * N*N));

	osg::ref_ptr<osg::Geode> cylinderLateral = new osg::Geode;
	cylinderLateral->addDrawable(coneLateralGeometry.get());

	osg::ref_ptr<osg::MatrixTransform> bottomTransform = new osg::MatrixTransform;
	bottomTransform->setMatrix(osg::Matrix::translate(osg::Vec3(0, -height / 2.0f, 0)));
	bottomTransform->addChild(GetDisk(radius, osg::Vec3(0.0f, -1.0f, 0.0f)));

	osg::ref_ptr<osg::Group> cone = new osg::Group;
	cone->addChild(cylinderLateral);
	cone->addChild(bottomTransform);
	return cone.get();
}

osg::ref_ptr<osg::Group> GetPineTree() {
	osg::ref_ptr<osg::Group> pineTree = new osg::Group;

	//létrehozás a default paraméterekkel,
	osg::ref_ptr<osg::Group> treeCrown = GetCone(4, 1);
	osg::ref_ptr<osg::MatrixTransform> treeCrownTransform = new osg::MatrixTransform;
	//...majd eltolás height/2 + treeTrunkHeighttal, azaz (4/2 + 1)-gyel
	treeCrownTransform->setMatrix(osg::Matrix::translate(0, 3.0f, 0));
	treeCrownTransform->addChild(treeCrown);
	pineTree->addChild(treeCrownTransform);

	//létrehozás a default paraméterekkel,
	osg::ref_ptr<osg::Group> treeTrunk = GetCylinder(1.5, 3);
	osg::ref_ptr<osg::MatrixTransform> treeTrunkTransform = new osg::MatrixTransform;
	//...majd skálázás és eltolás (height/2-vel, mivel alapból úgy jelenik meg, hogy az origó a közepe)
	treeTrunkTransform->setMatrix(osg::Matrix::scale(1.0 / 6.0, 2.0 / 3.0 , 1.0 / 6.0) * osg::Matrix::translate(0, 0.5f, 0));
	treeTrunkTransform->addChild(treeTrunk);
	pineTree->addChild(treeTrunkTransform);
	pineTree->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
	return pineTree.get();
}

int main(int argc, char** argv)
{
	//tengelyek megnyújtása
	osg::ref_ptr<osg::Node> axes_node = osgDB::readNodeFile("axes.osgt");
	osg::ref_ptr<osg::MatrixTransform> axes_transform = new osg::MatrixTransform;
	axes_transform->setMatrix(osg::Matrix::scale(10, 10, 10));
	axes_transform->addChild(axes_node);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	//exportálásra
	osg::ref_ptr<osg::Group> a = new osg::Group;
	osg::ref_ptr<osg::Group> b = new osg::Group;

	osg::ref_ptr<osg::Group> treeGroup = GetPineTree();
	int N = 11;
	for (int i = 0; i < N; ++i) {

		osg::ref_ptr<osg::MatrixTransform> t = new osg::MatrixTransform;
		float s = 1.5 - (i * 0.05);
		t->setMatrix(
			osg::Matrix::scale(s, s, s) *
			osg::Matrix::translate(-15, 0, 0) *
			osg::Matrix::rotate(i * 2 * osg::PI / N, 0, 1, 0)
		);
		t->addChild(treeGroup);

		if (i == 0) {
			a->addChild(t);
			osg::ref_ptr<osg::MatrixTransform> exportATransform = new osg::MatrixTransform;
			exportATransform->setMatrix(osg::Matrix::translate(0, (s * (0.5 + 4)), 0));
			exportATransform->addChild(t);
			osgDB::writeNodeFile(*(exportATransform.get()), "a.obj");
		}

		if (i == N - 1) {
			b->addChild(t);
			osg::ref_ptr<osg::MatrixTransform> exportBTranform = new osg::MatrixTransform;
			exportBTranform->setMatrix(osg::Matrix::translate(0, (s * (0.5 + 4)), 0));
			exportBTranform->addChild(t);
			osgDB::writeNodeFile(*(exportBTranform.get()), "b.obj");
		}
			
		root->addChild(t);
	}

	/*
	osg::ref_ptr<osg::ShapeDrawable> ground = new osg::ShapeDrawable;
	ground->setShape(new osg::Box(osg::Vec3(0.0f, 0.0f, 0.0f),	// csak segítségképp, hogy látszódjon, hogy szintben vannak a fák
		33.0f, 0.01f, 33.0f));						
	root->addChild(ground);
	//root->addChild(treeGroup);
	*/

	osgViewer::Viewer viewer;
	viewer.setSceneData(root.get());

	viewer.setUpViewInWindow(30, 30, 640, 480);
	viewer.getCamera()->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
	viewer.realize();


	return viewer.run();
}