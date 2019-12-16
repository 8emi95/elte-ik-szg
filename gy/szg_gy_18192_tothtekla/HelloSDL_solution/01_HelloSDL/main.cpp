#include <SDL.h>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>

#include <vector>

// line�ris interpol�ci� k�t pont k�z�tt
SDL_Point Line(double t) {
	SDL_Point A{50, 50};
	SDL_Point B{500, 100};

	return {
		int(t*(A.x) + (1 - t) * (B.x)), 
		int(t*(A.y) + (1 - t) * (B.y))
	};
}

// k�rvonal interpol�ci�ja adott C k�z�pponttal �s r sug�rral
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
	std::cout << "Kil�p�shez nyomj meg egy billenty�t..." << std::endl;
	std::cin.get();
}

int main( int argc, char* args[] )
{
	// �ll�tsuk be, hogy kil�p�s el�tt h�vja meg a rendszer az exitProgram() f�ggv�nyt
	atexit( exitProgram );

	//
	// 1. l�p�s: inicializ�ljuk az SDL-t
	//

	// a grafikus alrendszert kapcsoljuk csak be, ha gond van, akkor jelezz�k �s l�pj�nk ki
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


	// aktu�lis rajzol�si sz�n legyen fekete �s t�r�lj�k az aktu�lis rajzol�si sz�nnel az ablak klienster�let�t
	SDL_SetRenderDrawColor(	ren,	// melyik renderel�nek �ll�tjuk be az aktu�lis rajzol�si sz�n�t
							0,	// v�r�s intenzit�s - 8 bites el�jel n�lk�li eg�sz sz�m
							0,		// z�ld intenzit�s - 8 bites el�jel n�lk�li eg�sz sz�m
							0,		// k�k intenzit�s - 8 bites el�jel n�lk�li eg�sz sz�m
							255);	// �tl�tsz�s�g - 8 bites el�jel n�lk�li eg�sz sz�m
	SDL_RenderClear(ren);

	// aktu�lis rajzol�si sz�n legyen z�ld �s rajzoljunk ki egy vonalat
	SDL_SetRenderDrawColor(	ren,	// renderer c�me, aminek a rajzol�si sz�n�t be akarjuk �ll�tani
							0,		// piros
							255,	// z�ld
							0,		// k�k
							255);	// �tl�tszatlans�g

	// eg�sz�ts�k ki a fenti vonalat egy H bet�v�
	
	SDL_RenderDrawLine(	ren,	// renderer c�me, ahov� vonalat akarunk rajzolni
						10, 10, // vonal kezd�pontj�nak (x,y) koordin�t�i
						10, 60);// vonal v�gpontj�nak (x,y) koordin�t�i

	SDL_RenderDrawLine(ren,	// renderer c�me, ahov� vonalat akarunk rajzolni
						30, 10, // vonal kezd�pontj�nak (x,y) koordin�t�i
						30, 60);// vonal v�gpontj�nak (x,y) koordin�t�i

	SDL_RenderDrawLine(ren,	// renderer c�me, ahov� vonalat akarunk rajzolni
						10, 35, // vonal kezd�pontj�nak (x,y) koordin�t�i
						30, 35);// vonal v�gpontj�nak (x,y) koordin�t�i

	// Rajzoljunk parametrikus vonalat �s k�rt
	std::vector<SDL_Point> points_line; // az egyenes pontjait t�rol� t�mb
	std::vector<SDL_Point> points_circle; // a k�rvonal pontjait t�rol� t�mb
	const int STEPS = 20; // ennyi l�p�sk�zzel rajzoljuk ki a parametrikus alakzatokat

	for (int i = 0; i <= STEPS; i++) {
	
		double t = double(i) / double(STEPS); // t=[0;1]

		// Gener�lunk egy-egy pontot a parametrikus egyenletek alapj�n
		SDL_Point p_l = Line(t);
		SDL_Point p_c = Circle(t);

		// Hozz�adjuk a pontokat a megfelel� vektorhoz
		points_line.push_back(p_l);
		points_circle.push_back(p_c);
	}

	// kirajzoljuk a t�rolt pontokb�l alkotott t�r�ttvonalat
	SDL_RenderDrawLines(ren, points_line.data(), points_line.size());
	SDL_RenderDrawLines(ren, points_circle.data(), points_circle.size());

	// jelen�ts�k meg a backbuffer tartalm�t
	SDL_RenderPresent(ren);

	// v�rjunk 2 m�sodpercet
	SDL_Delay(2000);

	// 2 m�sodpercenk�nt v�ltozzon a h�tt�r sz�ne! El�sz�r legyen piros, azt�n z�ld �s v�g�l k�k,
	// majd l�pjen ki a program!

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
	// 4. l�p�s: l�pj�nk ki
	// 

	SDL_DestroyRenderer( ren );
	SDL_DestroyWindow( win );

	SDL_Quit();

	return 0;
}