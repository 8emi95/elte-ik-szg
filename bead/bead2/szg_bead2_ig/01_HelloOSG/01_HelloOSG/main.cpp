#include <osg/Texture2D>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osg/PolygonMode>
#include <osg/MatrixTransform>
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


void kupf(float i, osg::ref_ptr<osg::Vec3Array>& vertices,
	osg::ref_ptr<osg::Vec3Array>& normals,
	osg::ref_ptr<osg::Vec2Array>& texcoords
)
{
	static float r = 2;


	float fi = i * 2 * osg::PI;
	vertices->push_back(osg::Vec3(r * cos(fi), 0, r * sin(fi)));
	normals->push_back(osg::Vec3(r*cos(fi), 0, r*sin(fi)));
	texcoords->push_back(osg::Vec2(cos(fi), sin(fi)));

}


osg::ref_ptr<osg::Geode> genKup()
{
	// poz�ci�k
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

	const int N = 32;
	float delta = 1.0 / N;
	for (int i = 0; i<N; ++i) {
		float x = i*delta;
		kupf(i * delta, vertices, normals, texcoords);
		kupf(i * delta+delta, vertices, normals, texcoords);

		
		//a cs�csponttal k�sse �ssze*/
		vertices->push_back(osg::Vec3(0, 3, 0));
		normals->push_back(osg::Vec3(0, 1, 0));
		texcoords->push_back(osg::Vec2(0, 0));
		
	}


	// k�p geometria
	osg::ref_ptr<osg::Geometry> kup = new osg::Geometry;
	kup->setUseVertexBufferObjects(true);

	// �ll�tsuk be, hogy a VBO-ba milyen adatok ker�ljenek
	kup->setVertexArray(vertices.get());
	kup->setNormalArray(normals.get());
	kup->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	kup->setTexCoordArray(0, texcoords.get());
	// kirajzoland� primit�v meghat�roz�sa
	kup->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 6 * N)); // (ciklus 0-t�l N-1-ig) * (6 pont ciklusonk�nt)

	osg::ref_ptr<osg::Geode> kupgeode = new osg::Geode;
	kupgeode->addDrawable(kup.get());

	return kupgeode;
}

void calc(float x, float y,
	osg::ref_ptr<osg::Vec3Array>& vertices,
	osg::ref_ptr<osg::Vec3Array>& normals,
	osg::ref_ptr<osg::Vec2Array>& texcoords
) {
	float u = -2 * osg::PI * x;
	float v = 2 * osg::PI * y;

	//toruszhoz meg
	float R = 5;	//5 sugar� k�rvonal ment�n forgatunk
	float a = 1;	//t�rusz vastags�ga 1 sugar� k�r

	

	
	vertices->push_back(osg::Vec3((R + a*cos(v))* cos(u), a*sin(v), (R + a*cos(v))*sin(u)));	
	normals->push_back(osg::Vec3((a*cos(v))* cos(u), a*sin(v), (a*cos(v))*sin(u)));	
	
	texcoords->push_back(osg::Vec2(x, y));
}

int main(int argc, char** argv)
{
	// poz�ci�k
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;	//poz�ci�k itt lesznek
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array;
	osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array;

	const int N = 50;
	float delta = 1.0f / N;
	for (int i = 0; i<N; ++i) {
		for (int j = 0; j<N; ++j) {
			float x = i*delta;
			float y = j*delta;
			// 1. h�romsz�g: x,y x+delta,y y+delta,x
			calc(x, y, vertices, normals, texcoords);
			calc(x, y + delta, vertices, normals, texcoords);
			calc(x + delta, y, vertices, normals, texcoords);
			// 2. h�romsz�g: x+delta,y x+delta,y+delta y+delta,x
			calc(x + delta, y, vertices, normals, texcoords);
			calc(x, y + delta, vertices, normals, texcoords);
			calc(x + delta, y + delta, vertices, normals, texcoords);

		}
	}

	// n�gysz�g geometria
	osg::ref_ptr<osg::Geometry> quad = new osg::Geometry;
	quad->setUseVertexBufferObjects(true);

	// �ll�tsuk be, hogy a VBO-ba milyen adatok ker�ljenek
	quad->setVertexArray(vertices.get());
	quad->setNormalArray(normals.get());
	quad->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);
	quad->setTexCoordArray(0, texcoords.get());
	// kirajzoland� primit�v meghat�roz�sa
	quad->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, 6 * N*N));
	



	// t�rusz
	osg::ref_ptr<osg::Geode> torusz = new osg::Geode;
	torusz->addDrawable(quad.get());

	//k�p
	osg::ref_ptr<osg::Geode> kup = genKup();

	// 5 magas k�pp� alak�t�s
	osg::ref_ptr<osg::MatrixTransform> kupmeretezes = new osg::MatrixTransform;
	kupmeretezes->setMatrix(osg::Matrix::scale(1, 1 / 3.f * 5, 1)); // 3 magasb�l 1 magasat, majd 5
	kupmeretezes->addChild(kup);


	// 2 sugar� t�russz� alak�t�s
	osg::ref_ptr<osg::MatrixTransform> toruszmeretezes = new osg::MatrixTransform;
	toruszmeretezes->setMatrix(osg::Matrix::scale(1 / 5.f * 2, 0.5f, 1 / 5.f * 2)); // 5 sug�rb�l 1 sugarat, majd 2
	toruszmeretezes->addChild(torusz);

	// egyes�t
	osg::ref_ptr<osg::Group> egesz = new osg::Group;
	egesz->addChild(kupmeretezes);
	egesz->addChild(toruszmeretezes);


	/*
	osg::ref_ptr<osg::MatrixTransform> egesz = new osg::MatrixTransform;
	egesz->setMatrix(osg::Matrix::translate(0, 0.5, 0));  //0.5 kell az Y-ra, �s akkor lesz y=0-n (az 1-es vastags�ga miatt)
	egesz->addChild(egesz2);
	*/
	//bal
	const double balMeret = 0.75;
	osg::ref_ptr<osg::MatrixTransform> egeszBal = new osg::MatrixTransform;
	egeszBal->setMatrix(
		osg::Matrix::scale(balMeret, balMeret, balMeret)
		*osg::Matrix::translate(-10, -10, -10)
	);

	egeszBal->addChild(egesz);


	//jobb
	const double jobbMeret = 1.4;
	osg::ref_ptr<osg::MatrixTransform> egeszJobb = new osg::MatrixTransform;
	egeszJobb->setMatrix(
		osg::Matrix::scale(jobbMeret, jobbMeret, jobbMeret)
		* osg::Matrix::translate(10, 10, 10)
	);

	egeszJobb->addChild(egesz);


	/*
	//axes
	osg::ref_ptr<osg::Node> ax = osgDB::readNodeFile("axes.osgt");
	osg::ref_ptr<osg::MatrixTransform> axNagyit = new osg::MatrixTransform;
	axNagyit->setMatrix(osg::Matrix::scale(5,5,5));
	axNagyit->addChild(ax);
	*/



	// root
	osg::ref_ptr<osg::Group> root = new osg::Group;
	root->addChild(egesz);
	root->addChild(egeszBal);
	root->addChild(egeszJobb);
	
	//root->addChild(axNagyit);


	//t�rusz vastags�ga alapb�l 1, azaz alapb�l 10+1/2 az y = 0
	osg::ref_ptr<osg::MatrixTransform> egeszBalExp = new osg::MatrixTransform; //kisebbik 0.75
	egeszBalExp->setMatrix(
		osg::Matrix::translate(0, 10+0.75/2., 0) //saj�t poz�ci�j�hoz k�pest (10+a t�rusz vastags�g�nak fele: 0.75/2) -vel felcs�sztatom Y-on
		* osg::Matrix::rotate(osg::PI / 2.0, osg::X_AXIS)
	);
	egeszBalExp->addChild(egeszBal);

	osg::ref_ptr<osg::MatrixTransform> egeszJobbExp = new osg::MatrixTransform;//nagyobbik 1.4
	egeszJobbExp->setMatrix(
		osg::Matrix::translate(0, -10 + 1.4 / 2., 0) //saj�t poz�ci�j�hoz k�pest (10-a t�rusz vastags�g�nak fele: 1.4/2) -vel lecs�sztatom Y-on
		* osg::Matrix::rotate(osg::PI /2.0, osg::X_AXIS)
	);

	egeszJobbExp->addChild(egeszJobb);



	// hozzuk l�tre a viewer-t �s �ll�tsuk be a gy�keret megjelen�tend� adatnak
	osgViewer::Viewer viewer;
	viewer.setSceneData(root.get());

	// a (20,20) kezdeti poz�ci�ba hozzunk l�tre egy 640x480-as ablakot
	viewer.setUpViewInWindow(20, 20, 640, 480);
	viewer.realize();

	// �rjuk ki egy obj f�jlba a parametrikus fel�let�nket!
	osgDB::writeNodeFile(*(egeszJobbExp.get()), "a.obj");
	osgDB::writeNodeFile(*(egeszBalExp.get()), "b.obj");

	// adjuk �t a vez�rl�st a viewer-nek
	return viewer.run();
}