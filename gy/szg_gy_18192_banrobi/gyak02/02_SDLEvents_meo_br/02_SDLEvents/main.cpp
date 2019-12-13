#include <SDL.h>
#include <iostream>

void exitProgram()
{
	std::cout << "Kil�p�shez nyomj meg egy billenty�t..." << std::endl;
	std::cin.get();
}

int main( int argc, char* args[] )
{
	atexit( exitProgram );
	//
	// 1. l�p�s: inicializ�ljuk az SDL-t
	//

	// a grafikus alrendszert kapcsoljuk csak be, ha gond van, akkor jelezz�k �s l�pj�n ki
	if ( SDL_Init( SDL_INIT_VIDEO ) == -1 )
	{
		// irjuk ki a hibat es terminaljon a program
		std::cout << "[SDL ind�t�sa]Hiba az SDL inicializ�l�sa k�zben: " << SDL_GetError() << std::endl;
		return 1;
	}
			
	//
	// 2. l�p�s: hozzuk l�tre az ablakot, amire rajzolni fogunk
	//

	SDL_Window *win = nullptr;
    win = SDL_CreateWindow( "Hello SDL!",				// az ablak fejl�ce
							100,						// az ablak bal-fels� sark�nak kezdeti X koordin�t�ja
							100,						// az ablak bal-fels� sark�nak kezdeti Y koordin�t�ja
							640,						// ablak sz�less�ge
							480,						// �s magass�ga
							SDL_WINDOW_SHOWN);			// megjelen�t�si tulajdons�gok

	// ha nem siker�lt l�trehozni az ablakot, akkor �rjuk ki a hib�t, amit kaptunk �s l�pj�nk ki
    if (win == nullptr)
	{
		std::cout << "[Ablak l�trehoz�sa]Hiba az SDL inicializ�l�sa k�zben: " << SDL_GetError() << std::endl;
        return 1;
    }

	//
	// 3. l�p�s: hozzunk l�tre egy renderel�t, rajzol�t
	//

    SDL_Renderer *ren = nullptr;
    ren = SDL_CreateRenderer(	win, // melyik ablakhoz rendelj�k hozz� a renderert
								-1,  // melyik index� renderert inicializ�ljuka
									 // a -1 a harmadik param�terben meghat�rozott ig�nyeinknek megfelel� els� renderel�t jelenti
								SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);	// az ig�nyeink, azaz
																						// hardveresen gyors�tott �s vsync-et bev�r�
    if (ren == nullptr)
	{
        std::cout << "[Renderer l�trehoz�sa]Hiba az SDL inicializ�l�sa k�zben: " << SDL_GetError() << std::endl;
        return 1;
    }

	//
	// 4. l�p�s: ind�tsuk el a f� �zenetfeldolgoz� ciklust
	// 

	// v�get kell-e �rjen a program fut�sa?
	bool quit = false;
	// feldolgozand� �zenet ide ker�l
	SDL_Event ev;
	// eg�r X �s Y koordin�t�i
	Sint32 mouseX = 0, mouseY = 0;

	// MEO ------------------------------------------------------------------------------------------------------------------------

	int r = 0, g = 0, b = 0;

	// MEO ------------------------------------------------------------------------------------------------------------------------

	while (!quit)
	{
		// am�g van feldolgozand� �zenet dolgozzuk fel mindet:
		while ( SDL_PollEvent(&ev) )
		{
			switch (ev.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if ( ev.key.keysym.sym == SDLK_ESCAPE )
					quit = true;
				break;
			case SDL_MOUSEMOTION:
				mouseX = ev.motion.x;
				mouseY = ev.motion.y;
				break;

			// MEO ------------------------------------------------------------------------------------------------------------------------

			case SDL_MOUSEBUTTONDOWN:
				switch (ev.button.button) {
				case SDL_BUTTON_LEFT:
					r = 255;
					break;
				case SDL_BUTTON_MIDDLE:
					g = 255;
					break;
				case SDL_BUTTON_RIGHT:
					b = 255;
					break;
				}
				break;

			// MEO ------------------------------------------------------------------------------------------------------------------------

			case SDL_MOUSEBUTTONUP:
				// eg�rgomb felenged�s�nek esem�nye; a felengedett gomb a ev.button.button -ban tal�lhat�
				// a lehets�ges gombok a k�vetkez�ek: SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, 
				//		SDL_BUTTON_RIGHT, SDL_BUTTON_WHEELUP, SDL_BUTTON_WHEELDOWN

				// MEO ------------------------------------------------------------------------------------------------------------------------

				switch (ev.button.button) {
				case SDL_BUTTON_LEFT:
					r = 0;
					break;
				case SDL_BUTTON_MIDDLE:
					g = 0;
					break;
				case SDL_BUTTON_RIGHT:
					b = 0;
					break;

				// MEO ------------------------------------------------------------------------------------------------------------------------
				}
				break;
			}
		}

		// t�r�lj�k a h�tteret feh�rre
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderClear(ren);

		// aktu�lis rajzol�si sz�n legyen z�ld �s rajzoljunk ki egy vonalat
		SDL_SetRenderDrawColor(	ren,	// renderer c�me, aminek a rajzol�si sz�n�t be akarjuk �ll�tani
								0,		// piros
								255,	// z�ld
								0,		// k�k
								255);	// �tl�tszatlans�g

		SDL_RenderDrawLine(	ren,	// renderer c�me, ahov� vonalat akarunk rajzolni
							0, 0, // vonal kezd�pontj�nak (x,y) koordin�t�i
							mouseX, mouseY);// vonal v�gpontj�nak (x,y) koordin�t�i
		
		// defini�ljunk egy (mouseX, mouseY) k�z�ppont�, tengelyekkel p�rhuzamos oldal�
		// 20x20-as n�gyzetet:


		// MEO ------------------------------------------------------------------------------------------------------------------------
		
		float t = SDL_GetTicks() / 1000.0f *M_PI / 2;

		int side = 40 * sin(t) + 60; // 40 - intervallum, 60 - gyorsas�g

		SDL_Rect cursor_rect;
		cursor_rect.x = mouseX - side/2; // 10 helyett
		cursor_rect.y = mouseY - side/2; // 10 helyett
		cursor_rect.w = side; // 20 helyett
		cursor_rect.h = side; // 20 helyett

		// legyen a kit�lt�si sz�n piros
		SDL_SetRenderDrawColor(ren, r, g, b, 255);
		// ha r, g, b bool:
		//SDL_SetRenderDrawColor(ren, r ? 255 : 0, g ? 255 : 0, b ? 255 : 0, 255);

		SDL_RenderFillRect( ren, &cursor_rect);

		// �tsz�g
		int R = side;
		const int N = 5;
		float alpha0 = 2 * M_PI / N;
		SDL_Point points[N + 1];
		for (int i = 0; i <= N; ++i) {
			float alpha = i*alpha0;
			int x = mouseX + R * cos(alpha);
			int y = mouseY + R * sin(alpha);
			points[i] = { x, y };
		}
		SDL_RenderDrawLines(ren, points, N + 1);

		// MEO ------------------------------------------------------------------------------------------------------------------------

		// 1. feladat: az eltelt id� f�ggv�ny�ben periodikusan n�jj�n �s cs�kkenjen
		//    az eg�rmutat� k�z�ppontj�val kirajzolt n�gysz�g

		// 2. feladat: ha a user a bal eg�rgombot nyomja meg akkor a t�glalap sz�ne v�ltson pirosra,
		//    ha a jobb eg�rgombot, akkor k�kre

		// 3. feladat: rajzolj ki egy 50 sugar� k�rvonalat az eg�rmutat� k�r�!
		// seg�ts�g: haszn�ld a SDL_RenderDrawLines()-t

		// jelen�ts�k meg a backbuffer tartalm�t
		SDL_RenderPresent(ren);
	}



	//
	// 4. l�p�s: l�pj�nk ki
	// 

	SDL_DestroyRenderer( ren );
	SDL_DestroyWindow( win );

	SDL_Quit();

	return 0;
}