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

	if ( SDL_Init( SDL_INIT_VIDEO ) == -1 )
	{
		std::cout << "[SDL ind�t�sa]Hiba az SDL inicializ�l�sa k�zben: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window *win = nullptr;
    win = SDL_CreateWindow( "Hello SDL!", 100, 100,	640, 480, SDL_WINDOW_SHOWN);		
    if (win == nullptr)
	{
		std::cout << "[Ablak l�trehoz�sa]Hiba az SDL inicializ�l�sa k�zben: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer *ren = nullptr;
    ren = SDL_CreateRenderer(win, -1,  SDL_RENDERER_ACCELERATED /*SDL_RENDERER_PRESENTVSYNC*/);	
    if (ren == nullptr)
	{
        std::cout << "[Renderer l�trehoz�sa]Hiba az SDL inicializ�l�sa k�zben: " << SDL_GetError() << std::endl;
        return 1;
    }

	//
	// 4. l�p�s: ind�tsuk el a f� �zenetfeldolgoz� ciklust
	// 

	// v�get kell-e �rjen a program fut�sa? (allapotvaltozo)
	bool quit = false;
	// feldolgozand� �zenet ide ker�l
	SDL_Event ev;
	// eg�r X �s Y koordin�t�i 
	Sint32 mouseX = 0, mouseY = 0;
	//allapotvaltozo
	bool isRed = true;
	bool shouldBeFaster = false;
	bool shouldBeSlower = false;
	bool shouldBeTheSame = false;
	float v = 1.0;

	while (!quit) //amig nem akarunk kilepni <-> amig fut a program
	{
		// am�g van feldolgozand� �zenet dolgozzuk fel mindet:
		while ( SDL_PollEvent(&ev) )
		{
			switch (ev.type) //nezzuk meg, milyen tipusu esemennyel van dolgunk
			{
			case SDL_QUIT: //ha az esemeny kilepest jelez
				quit = true; //akkor hagyjuk a picsaba az esemenyeket, mar nem akarjuk figyelni oket
				break;
			case SDL_KEYDOWN: //ha billentyuleutes tipusu az esemeny
				if ( ev.key.keysym.sym == SDLK_ESCAPE ) //es ha ez ESC volt
					quit = true;  //akkor szinten kilepunk
				if (ev.key.keysym.sym == SDLK_1)
				{
					shouldBeFaster = true;
					shouldBeSlower = false;
					shouldBeTheSame = false;
				}
				if (ev.key.keysym.sym == SDLK_2)
				{
					shouldBeFaster = false;
					shouldBeSlower = false;
					shouldBeTheSame = true;
				}
				if (ev.key.keysym.sym == SDLK_3)
				{
					shouldBeFaster = false;
					shouldBeSlower = true;
					shouldBeTheSame = false;
				}
				break;

			case SDL_MOUSEMOTION: //ha tapasztaltunk egermozgast, akkor lekerdezzuk az uj koordinatakat
				mouseX = ev.motion.x;
				mouseY = ev.motion.y;
				break;
			case SDL_MOUSEBUTTONDOWN: //ha lenyomtuk az egeret
				if (ev.button.button == SDL_BUTTON_LEFT) { //es ez a bal egergomb volt
					isRed = false; //akkor tudatjuk a programmal, hogy nem pirossal kell valamit csinalnia
				}
				break;
			case SDL_MOUSEBUTTONUP: //ha felengedjuk
				if (ev.button.button == SDL_BUTTON_LEFT) {
					isRed = true; //ismet piros
				}
				break;
			}
		}

		// t�r�lj�k a h�tteret feh�rre
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderClear(ren);


		SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);	// 255: �tl�tszatlans�g
		//ATTENTION! A while(!quit) ciklusban vagyunk. ezert kilepesig minden kirajzolas utan ujrakezdodik a folyamat, a megfelelo poziciokkal, allapotokkal
		//(0,0)-tol az eger poziciojaig akarunk rajzolni egy vonalat.
		//ez a pozicio minden egyes ciklusban lekerodik, tehat ha mozgunk, azannak megfelelo poziciokat kapjuk meg. ezert lenyegeben lesz egy valamink, ami koveti a kurzor mozgasat
		SDL_RenderDrawLine(	ren,	// renderer c�me, ahov� vonalat akarunk rajzolni
							0, 0, // vonal kezd�pontj�nak (x,y) koordin�t�i
							mouseX, mouseY);// vonal v�gpontj�nak (x,y) koordin�t�i
		
		//SDL_RenderPresent(ren);
		/*
		// defini�ljunk egy (mouseX, mouseY) k�z�ppontu, tengelyekkel p�rhuzamos oldal�, 20x20-as n�gyzetet:

		SDL_Rect cursor_rect; //letrehozunk egy teglalapot
		//beallitjuk a kezdopontjat
		cursor_rect.x = mouseX - 10; //mivel 20 oldalu, a kezdopontja a (kozeppont-10)
		cursor_rect.y = mouseY - 10;
		//beallitjuk a szelesseget, magassagat (most egy negyzet)
		cursor_rect.w = 20;
		cursor_rect.h = 20;
		*/

		//valtakozzon a merete periodikusan az ido fuggvenyeben - getticks
		int time = SDL_GetTicks(); //program futasa ota eltelt ido ms-ben
		//ennek fuggvenyeben fog majd valtozni a teglalap merete 10 es 50 kozott
		//sin(2*PI*t), 20*sin(2*PI*t)+30 --> az eltelt idovel megszorozva garantaljuk, hogy ez valoban valtozni fog, illetve a szinusz periodusa 2 PI, meg ez is szukseges
		//valamilyen faktorral szorozhatjuk is a szinusz belsejet, ezzel novelhetjuk a sebesseget vagy lassithatjuk azt. ha egy kicsi szammal, akkor egyre lassabban lepdel a szinusz, tehat lassabb lesz, ha nagyobb szammal, akkor hamarabb vegiger egy perioduson, igy gyorsabb

		if (shouldBeFaster)
		{
			v = 2.0;
		}
		if (shouldBeSlower)
		{
			v = 0.5;
		}
		if (shouldBeTheSame)
		{
			v = 1.0;
		}

		int s = sin(v*time / 1000.0 * 2 * M_PI)*20 + 30;

		SDL_Rect cursor_rect; 
		cursor_rect.x = mouseX - s/2; //s valamilyen szeles. ha azt akarjuk, hogy az kurzor legyen a kozeppont, akkor a kezdopontot ugy kapjuk meg, ha a kozeppontbol kivonjuk a valamilyen szelesseg felet
		cursor_rect.y = mouseY - s/2;
		cursor_rect.w = s; //szelessege az elobb kiszamolt s lesz, ami valtakozik
		cursor_rect.h = s; //szinten

		// ha piros
		if(isRed)
			SDL_SetRenderDrawColor( ren, 255, 0, 0, 255 );
		else //ha nem, akkor kekre allitjuk
			SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
		//kitoltjuk a teglalapot a megadott szinnel
		SDL_RenderFillRect( ren, &cursor_rect);

		// 1. feladat: az eltelt id� f�ggv�ny�ben periodikusan n�jj�n �s cs�kkenjen
		//    az eg�rmutat� k�z�ppontj�val kirajzolt n�gysz�g

		// 2. feladat: ha a user a bal eg�rgombot nyomja meg akkor a t�glalap sz�ne v�ltson pirosra,
		//    ha a jobb eg�rgombot, akkor k�kre

		// 3. feladat: rajzolj ki egy 50 sugar� k�rvonalat az eg�rmutat� k�r�!
		// seg�ts�g: haszn�ld a SDL_RenderDrawLines()-t

		// jelen�ts�k meg a backbuffer tartalm�t
		SDL_RenderPresent(ren);
	}


	//destroy: ren, win, sdl
	SDL_DestroyRenderer( ren );
	SDL_DestroyWindow( win );

	SDL_Quit();

	return 0;
}