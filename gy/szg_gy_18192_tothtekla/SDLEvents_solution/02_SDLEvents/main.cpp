#include <SDL.h>
#include <iostream>
#include <vector>
#include <cmath>

void exitProgram()
{
	std::cout << "Kilépéshez nyomj meg egy billentyût..." << std::endl;
	std::cin.get();
}


SDL_Point Circle(double t) {
	SDL_Point C = { 200, 200 };
	double r = 50;

	return {
		int(C.x + r * cos(t * M_PI * 2)),
		int(C.y + r * sin(t * M_PI * 2))
	};
}

SDL_Point Spiral(double t) {
	SDL_Point C = { 200, 300 };
	double r = 100;
	double round = 5;

	return {
		int(double(C.x) + (t*r) * cos(t * M_PI * 2 * round)),
		int(double(C.y) + (t*r) * sin(t * M_PI * 2 * round))
	};
}


int main(int argc, char* args[])
{
	atexit(exitProgram);
	//
	// 1. lépés: inicializáljuk az SDL-t
	//

	// a grafikus alrendszert kapcsoljuk csak be, ha gond van, akkor jelezzük és lépjün ki
	if (SDL_Init(SDL_INIT_VIDEO) == -1)
	{
		// irjuk ki a hibat es terminaljon a program
		std::cout << "[SDL indítása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
		return 1;
	}

	//
	// 2. lépés: hozzuk létre az ablakot, amire rajzolni fogunk
	//

	SDL_Window *win = nullptr;
	win = SDL_CreateWindow("Hello SDL!",				// az ablak fejléce
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
	ren = SDL_CreateRenderer(win, // melyik ablakhoz rendeljük hozzá a renderert
		-1,  // melyik indexú renderert inicializáljuka
			 // a -1 a harmadik paraméterben meghatározott igényeinknek megfelelõ elsõ renderelõt jelenti
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);	// az igényeink, azaz
																// hardveresen gyorsított és vsync-et beváró
	if (ren == nullptr)
	{
		std::cout << "[Renderer létrehozása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
		return 1;
	}

	//
	// 4. lépés: indítsuk el a fõ üzenetfeldolgozó ciklust
	// 

	// véget kell-e érjen a program futása?
	bool quit = false;
	// feldolgozandó üzenet ide kerül
	SDL_Event ev;
	// egér X és Y koordinátái
	Sint32 mouseX = 0, mouseY = 0;
	bool is_left_down = false;

	while (!quit)
	{
		// amíg van feldolgozandó üzenet dolgozzuk fel mindet:
		while (SDL_PollEvent(&ev))
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (ev.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
				break;
			case SDL_MOUSEMOTION:
				mouseX = ev.motion.x;
				mouseY = ev.motion.y;
				break;
			case SDL_MOUSEBUTTONUP:
				// egérgomb felengedésének eseménye; a felengedett gomb a ev.button.button -ban található
				// a lehetséges gombok a következõek: SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, 
				//		SDL_BUTTON_RIGHT, SDL_BUTTON_WHEELUP, SDL_BUTTON_WHEELDOWN
				break;
			}
		}

		// töröljük a hátteret fehérre
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderClear(ren);

		// aktuális rajzolási szín legyen zöld és rajzoljunk ki egy vonalat
		SDL_SetRenderDrawColor(ren,	// renderer címe, aminek a rajzolási színét be akarjuk állítani
			0,		// piros
			255,	// zöld
			0,		// kék
			255);	// átlátszatlanság

		SDL_RenderDrawLine(ren,	// renderer címe, ahová vonalat akarunk rajzolni
			0, 0, // vonal kezdõpontjának (x,y) koordinátái
			mouseX, mouseY);// vonal végpontjának (x,y) koordinátái

		// definiáljunk egy (mouseX, mouseY) középpontó, tengelyekkel párhuzamos oldalú
		// 20x20-as négyzetet:
		SDL_Rect cursor_rect;


		// legyen a kitöltési szín piros
		//SDL_SetRenderDrawColor( ren, 255, 0, 0, 255 );
		//SDL_RenderFillRect( ren, &cursor_rect);

		// 1. feladat: az eltelt idõ függvényében periodikusan nõjjön és csökkenjen
		//    az egérmutató középpontjával kirajzolt négyszög
		Uint32 time = SDL_GetTicks() % 2000;
		if (time > 1000) {
			time = 2000 - time;
		}
		time /= 10;
		cursor_rect.x = mouseX - time / 2;
		cursor_rect.y = mouseY - time / 2;
		cursor_rect.w = time;
		cursor_rect.h = time;

		// 2. feladat: ha a user a bal egérgombot nyomja meg akkor a téglalap színe váltson pirosra,
		//    ha a jobb egérgombot, akkor kékre

		SDL_RenderFillRect(ren, &cursor_rect);

		// 3. feladat: rajzolj ki egy 50 sugarú spirált az egérmutató köré!
		// segítség: használd a SDL_RenderDrawLines()-t

		const int STEPS = 100;
		std::vector<SDL_Point> points;

		for (int i = 0; i <= STEPS; i++) {
			double t = double(i) / double(STEPS);
			SDL_Point p = Spiral(t);
			points.push_back(p);
		}

		SDL_RenderDrawLines(ren, points.data(), points.size());


		// jelenítsük meg a backbuffer tartalmát
		SDL_RenderPresent(ren);
	}


	//
	// 4. lépés: lépjünk ki
	// 

	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);

	SDL_Quit();

	return 0;
}