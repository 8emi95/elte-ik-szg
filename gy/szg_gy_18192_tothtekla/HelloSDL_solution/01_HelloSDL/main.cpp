#include <SDL.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include <vector>

// lineáris interpoláció két pont között
SDL_Point Line(double t) {
	SDL_Point A{50, 50};
	SDL_Point B{500, 100};

	return {
		int(t*(A.x) + (1 - t) * (B.x)), 
		int(t*(A.y) + (1 - t) * (B.y))
	};
}

// körvonal interpolációja adott C középponttal és r sugárral
SDL_Point Circle(double t) {
	SDL_Point C{ 200, 200 };
	double r = 100;

	return {
		int(r*sin(t*2*M_PI)) + C.x,
		int(r*cos(t*2*M_PI)) + C.y
	};
}

void exitProgram()
{
	std::cout << "Kilépéshez nyomj meg egy billentyût..." << std::endl;
	std::cin.get();
}

int main( int argc, char* args[] )
{
	// állítsuk be, hogy kilépés elõtt hívja meg a rendszer az exitProgram() függvényt
	atexit( exitProgram );

	//
	// 1. lépés: inicializáljuk az SDL-t
	//

	// a grafikus alrendszert kapcsoljuk csak be, ha gond van, akkor jelezzük és lépjünk ki
	if ( SDL_Init( SDL_INIT_VIDEO ) == -1 )
	{
		// irjuk ki a hibat es terminaljon a program
		std::cout << "[SDL indítása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
		return 1;
	}
			
	//
	// 2. lépés: hozzuk létre az ablakot, amire rajzolni fogunk
	//

	SDL_Window *win = nullptr;
    win = SDL_CreateWindow( "Hello SDL!",				// az ablak fejléce
							100,						// az ablak bal-felsõ sarkának kezdeti X koordinátája
							100,						// az ablak bal-felsõ sarkának kezdeti Y koordinátája
							640,						// ablak szélessége
							480,						// és magassága
							SDL_WINDOW_SHOWN);			// megjelenítési tulajdonságok

	// ha nem sikerült létrehozni az ablakot, akkor írjuk ki a hibát, amit kaptunk és lépjünk ki
    if (win == nullptr)
	{
		std::cout << "[Ablak létrehozása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
        return 1;
    }

	//
	// 3. lépés: hozzunk létre egy renderelõt, rajzolót
	//

    SDL_Renderer *ren = nullptr;
    ren = SDL_CreateRenderer(	win, // melyik ablakhoz rendeljük hozzá a renderert
								-1,  // melyik indexú renderert inicializáljuka
									 // a -1 a harmadik paraméterben meghatározott igényeinknek megfelelõ elsõ renderelõt jelenti
								SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);	// az igényeink, azaz
																						// hardveresen gyorsított és vsync-et beváró
    if (ren == nullptr)
	{
        std::cout << "[Renderer létrehozása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
        return 1;
    }


	// aktuális rajzolási szín legyen fekete és töröljük az aktuális rajzolási színnel az ablak kliensterületét
	SDL_SetRenderDrawColor(	ren,	// melyik renderelõnek állítjuk be az aktuális rajzolási színét
							0,	// vörös intenzitás - 8 bites elõjel nélküli egész szám
							0,		// zöld intenzitás - 8 bites elõjel nélküli egész szám
							0,		// kék intenzitás - 8 bites elõjel nélküli egész szám
							255);	// átlátszóság - 8 bites elõjel nélküli egész szám
	SDL_RenderClear(ren);

	// aktuális rajzolási szín legyen zöld és rajzoljunk ki egy vonalat
	SDL_SetRenderDrawColor(	ren,	// renderer címe, aminek a rajzolási színét be akarjuk állítani
							0,		// piros
							255,	// zöld
							0,		// kék
							255);	// átlátszatlanság

	// egészítsük ki a fenti vonalat egy H betûvé
	
	SDL_RenderDrawLine(	ren,	// renderer címe, ahová vonalat akarunk rajzolni
						10, 10, // vonal kezdõpontjának (x,y) koordinátái
						10, 60);// vonal végpontjának (x,y) koordinátái

	SDL_RenderDrawLine(ren,	// renderer címe, ahová vonalat akarunk rajzolni
						30, 10, // vonal kezdõpontjának (x,y) koordinátái
						30, 60);// vonal végpontjának (x,y) koordinátái

	SDL_RenderDrawLine(ren,	// renderer címe, ahová vonalat akarunk rajzolni
						10, 35, // vonal kezdõpontjának (x,y) koordinátái
						30, 35);// vonal végpontjának (x,y) koordinátái

	// Rajzoljunk parametrikus vonalat és kört
	std::vector<SDL_Point> points_line; // az egyenes pontjait tároló tömb
	std::vector<SDL_Point> points_circle; // a körvonal pontjait tároló tömb
	const int STEPS = 20; // ennyi lépésközzel rajzoljuk ki a parametrikus alakzatokat

	for (int i = 0; i <= STEPS; i++) {
	
		double t = double(i) / double(STEPS); // t=[0;1]

		// Generálunk egy-egy pontot a parametrikus egyenletek alapján
		SDL_Point p_l = Line(t);
		SDL_Point p_c = Circle(t);

		// Hozzáadjuk a pontokat a megfelelõ vektorhoz
		points_line.push_back(p_l);
		points_circle.push_back(p_c);
	}

	// kirajzoljuk a tárolt pontokból alkotott töröttvonalat
	SDL_RenderDrawLines(ren, points_line.data(), points_line.size());
	SDL_RenderDrawLines(ren, points_circle.data(), points_circle.size());

	// jelenítsük meg a backbuffer tartalmát
	SDL_RenderPresent(ren);

	// várjunk 2 másodpercet
	SDL_Delay(2000);

	// 2 másodpercenként változzon a háttér színe! Elõször legyen piros, aztán zöld és végül kék,
	// majd lépjen ki a program!

	SDL_SetRenderDrawColor(ren, 255, 0, 0, 255); //R
	SDL_RenderClear(ren);
	SDL_RenderPresent(ren);
	SDL_Delay(2000);

	SDL_SetRenderDrawColor(ren, 0, 255, 0, 255); //G
	SDL_RenderClear(ren);
	SDL_RenderPresent(ren);
	SDL_Delay(2000);

	SDL_SetRenderDrawColor(ren, 0, 0, 255, 255); //B
	SDL_RenderClear(ren);
	SDL_RenderPresent(ren);
	SDL_Delay(2000);

	//
	// 4. lépés: lépjünk ki
	// 

	SDL_DestroyRenderer( ren );
	SDL_DestroyWindow( win );

	SDL_Quit();

	return 0;
}