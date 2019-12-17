#include <SDL.h>
#include <iostream>

void exitProgram()
{
	std::cout << "Kilépéshez nyomj meg egy billentyût..." << std::endl;
	std::cin.get();
}

int main( int argc, char* args[] )
{
	atexit( exitProgram );

	if ( SDL_Init( SDL_INIT_VIDEO ) == -1 )
	{
		std::cout << "[SDL indítása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window *win = nullptr;
    win = SDL_CreateWindow( "Hello SDL!", 100, 100,	640, 480, SDL_WINDOW_SHOWN);		
    if (win == nullptr)
	{
		std::cout << "[Ablak létrehozása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer *ren = nullptr;
    ren = SDL_CreateRenderer(win, -1,  SDL_RENDERER_ACCELERATED /*SDL_RENDERER_PRESENTVSYNC*/);	
    if (ren == nullptr)
	{
        std::cout << "[Renderer létrehozása]Hiba az SDL inicializálása közben: " << SDL_GetError() << std::endl;
        return 1;
    }

	//
	// 4. lépés: indítsuk el a fõ üzenetfeldolgozó ciklust
	// 

	// véget kell-e érjen a program futása? (allapotvaltozo)
	bool quit = false;
	// feldolgozandó üzenet ide kerül
	SDL_Event ev;
	// egér X és Y koordinátái 
	Sint32 mouseX = 0, mouseY = 0;
	//allapotvaltozo
	bool isRed = true;
	bool shouldBeFaster = false;
	bool shouldBeSlower = false;
	bool shouldBeTheSame = false;
	float v = 1.0;

	while (!quit) //amig nem akarunk kilepni <-> amig fut a program
	{
		// amíg van feldolgozandó üzenet dolgozzuk fel mindet:
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

		// töröljük a hátteret fehérre
		SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
		SDL_RenderClear(ren);


		SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);	// 255: átlátszatlanság
		//ATTENTION! A while(!quit) ciklusban vagyunk. ezert kilepesig minden kirajzolas utan ujrakezdodik a folyamat, a megfelelo poziciokkal, allapotokkal
		//(0,0)-tol az eger poziciojaig akarunk rajzolni egy vonalat.
		//ez a pozicio minden egyes ciklusban lekerodik, tehat ha mozgunk, azannak megfelelo poziciokat kapjuk meg. ezert lenyegeben lesz egy valamink, ami koveti a kurzor mozgasat
		SDL_RenderDrawLine(	ren,	// renderer címe, ahová vonalat akarunk rajzolni
							0, 0, // vonal kezdõpontjának (x,y) koordinátái
							mouseX, mouseY);// vonal végpontjának (x,y) koordinátái
		
		//SDL_RenderPresent(ren);
		/*
		// definiáljunk egy (mouseX, mouseY) középpontu, tengelyekkel párhuzamos oldalú, 20x20-as négyzetet:

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

		// 1. feladat: az eltelt idõ függvényében periodikusan nõjjön és csökkenjen
		//    az egérmutató középpontjával kirajzolt négyszög

		// 2. feladat: ha a user a bal egérgombot nyomja meg akkor a téglalap színe váltson pirosra,
		//    ha a jobb egérgombot, akkor kékre

		// 3. feladat: rajzolj ki egy 50 sugarú körvonalat az egérmutató köré!
		// segítség: használd a SDL_RenderDrawLines()-t

		// jelenítsük meg a backbuffer tartalmát
		SDL_RenderPresent(ren);
	}


	//destroy: ren, win, sdl
	SDL_DestroyRenderer( ren );
	SDL_DestroyWindow( win );

	SDL_Quit();

	return 0;
}