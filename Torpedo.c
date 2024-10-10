#include <stdio.h>
#include <time.h> // randomhoz kell

struct Segment{
    char ShipID[2];
    int Shot;
    int RelativeCoordinates[2];
};

struct Ship{
    char ID[2];
    //majd a jövőben ID[x][2] mert így csak 0-9-ig megy az id ha jól sejtem
    int SegmentCount;
    int Coordinates[2];
    struct Segment *Segments[4];
    int Sunk;
    /*nem engedte hogy Segments[SegmentCount],
    Ezért 4-nél nem lehet több, szóval 4 hosszú*/
};

struct Fleet{
    struct Ship Ships[10];
    int ShipCount;
};

struct Segment MapPlayer[10][10];
struct Segment MapEnemy[10][10];

struct Fleet FleetPlayer = {0};
struct Fleet FleetEnemy = {0};

//Kiiratás
void BorderLine(){
    for(int i = 0; i < 20; i++){
        printf("\u2550");
    }
}

void DrawTop(int single){
    printf("  1 2 3 4 5 6 7 8 9 10");
    if(single == 0){
        printf("\t   1 2 3 4 5 6 7 8 9 10\n");
    }
    else{
        printf("\n");
    }
    printf(" \u2554");
    BorderLine();
    printf("\u2557");
    if(single == 0){
        printf("\t  \u2554");
        BorderLine();
        printf("\u2557\n");
    }
    else{
        printf("\n");
    }
}

void DrawBottom(int single){
    printf(" \u255A");
    BorderLine();
    printf("\u255D");
    if(single == 0){
        printf("\t  \u255A");
        BorderLine();
        printf("\u255D\n");
    }
    else{
        printf("\n");
    }
}

void DrawRow(struct Segment map[10], int player, int abc, int single, int end){
    printf("%c\u2551", abc + '0');
    for(int i = 0; i < 10; i++){
        if(map[i].ShipID[0] == '-' || (player == 0 && map[i].Shot == 0)){
            
            if(map[i].Shot == 0){
                printf("\033[34;104m~\u224B\033[0m");
            }
            else{
                printf("\033[30;104m()\033[0m");
            }
        }
        else if(map[i].Shot == 0 && player == 1){
            printf("\033[30;100m[]\033[0m");
        }
        else{
            printf("\033[30;101m><\033[0m");
        }
    }
    printf("\u2551");

    if(player == 1 && single == 0 && end == 0){
        printf("\t ");
    }
    else{
        printf("\n");
    }
}

void DrawMap(int single, int end){
    printf("\e[1;1H\e[2J"); //Képernyő clear

    DrawTop(single);
    for(int i = 0; i < 10; i++){

        DrawRow(MapPlayer[i], 1,i + 17,single, 0);
        if(single == 0){
            DrawRow(MapEnemy[i], end,i + 17,single, end);
        }
    }
    DrawBottom(single);
}
//Kiiratás vége

//halyó helyezés
void CreateSegments(struct Ship *ship,int rotation, struct Segment map[10][10]){
    for(int i = 0; i < ship->SegmentCount; i++){
        struct Segment segment = {
            {ship->ID[0],ship->ID[1]}, //ShipID
            0, //Shot
            {0,0} //RelativeCoordinates
        };

        switch(rotation){
            case 0:
                segment.RelativeCoordinates[1] += i;
                break;
            case 1:
                segment.RelativeCoordinates[0] += i;
                break;
            case 2:
                segment.RelativeCoordinates[1] -= i;
                break;
            case 3:
                segment.RelativeCoordinates[0] -= i;
                break;
            default:
                break;
        }

        map[ship->Coordinates[0] + segment.RelativeCoordinates[0]][ship->Coordinates[1] + segment.RelativeCoordinates[1]] = segment;
        ship->Segments[i] = &map[ship->Coordinates[0] + segment.RelativeCoordinates[0]][ship->Coordinates[1] + segment.RelativeCoordinates[1]];

    }
}

void CreateShip(struct Fleet *thisFleet, char type, int coordinates[2], int rotation, int player){
    
    int segments;

    switch(type){
        case '1':
            segments = 1;
            break;
        case '2':
            segments = 2;
            break;
        case '3':
            segments = 3;
            break;
        case '4':
            segments = 4;
            break;
        case 'L':
            segments = 3;
            break;
        default:
            segments = 1;
            break;
    }

    struct Ship ship ={
        {thisFleet->ShipCount + '0',type}, //ID
        segments, //SegmentCount
        {coordinates[0],coordinates[1]}, //Coordinates
        {0}, //Segments (empty)
        0 //Sunk
    };
    if(player == 1){
        CreateSegments(&ship,rotation,MapPlayer);
    }
    else{
        CreateSegments(&ship,rotation,MapEnemy);

    }

    thisFleet->Ships[thisFleet->ShipCount] = ship;
    thisFleet->ShipCount++;
}

int LookAround(int cords[2], struct Segment map[10][10]){
    int possible[8][2] = {
        {0,1},  //0 = right
        {0,-1}, //1 = left
        {1,0},  //2 = down
        {-1,0}, //3 = up
        {1,1},  //4 = down-right
        {1,-1}, //5 = down-left
        {-1,-1},//6 = up-left
        {-1,1}  //7 = up-right
    };

    for(int i = 0; i < 8; i++){
        int tempcord[2] = {possible[i][0] + cords[0],possible[i][1] + cords[1]};
        if(tempcord[0] > -1 && tempcord[0] < 10 && tempcord[1] > -1 && tempcord[1] < 10){
            if(map[tempcord[0]][tempcord[1]].ShipID[0] != '-'){
                return 0;
            }
        }
    }

    return 1;
}

int CanPlace(struct Segment map[10][10], char type, int cords[2], int rotation){
    int segments;

    switch(type){
        case '1':
            segments = 1;
            break;
        case '2':
            segments = 2;
            break;
        case '3':
            segments = 3;
            break;
        case '4':
            segments = 4;
            break;
        case 'L':
            segments = 3;
            break;
        default:
            break;
    }

    int relativeCords[2] = {0,0};

    switch(rotation){
            case 0:
                relativeCords[1] += 1;
                break;
            case 1:
                relativeCords[0] += 1;
                break;
            case 2:
                relativeCords[1] -= 1;
                break;
            case 3:
                relativeCords[0] -= 1;
                break;
            default:
                break;
        }
    if(cords[0] + (relativeCords[0] * segments) < -1 || cords[0] + (relativeCords[0] * segments) > 10 || cords[1] + (relativeCords[1] * segments) < -1 || cords[1] + (relativeCords[1] * segments) > 10){
        return 0;
    }
    for(int i = 0; i < segments;i++){
        int currCord[2] = {cords[0] + relativeCords[0] * i, cords[1] + relativeCords[1] * i};
        if(map[currCord[0]][currCord[1]].ShipID[0] == '-'){
            if(LookAround(currCord,map) == 0){
                return 0;
            }
        }
        else{
            return 0;
        }
    }
    return 1;
}
//hajó helyezés vége

int RandomPlace(int player,char type){
    int cords[2] = {rand()%10,rand()%10};
    int rotation = rand()%4;

    if(player == 1){
        if(CanPlace(MapPlayer,type,cords,rotation) == 1){
            CreateShip(&FleetPlayer,type,cords,rotation,1);
            return 1;
        }
    }
    else{
        if(CanPlace(MapEnemy,type,cords,rotation) == 1){
            CreateShip(&FleetEnemy,type,cords,rotation,0);
            return 1;
        }
    }
    return 0;
}

void Setup(int player, int random){
    /* basic Setup
        type | number
        4 | 1
        3 | 1
        2 | 2
        1 | 2  
    */
    int numberOfShips = 10;
    char shipSet[10] = {
        '4',
        '4',
        '3',
        '3',
        '3',
        '2',
        '2',
        '2',
        '1',
        '1'
    };

    if(random == 0){
        for(int i = 0; i < numberOfShips; i++){
            DrawMap(1,0);
            int done = 0;
            while(done == 0){
                char type = shipSet[i];
                int cords[2];
                char cordRow;
                int rotation;
                
                printf("\nHelyezd el a(z) %c hosszú hajódat és forgását (0:jobbra,1:le...)\n A-J:1-10 0-3\n", type);
                scanf(" %c:%d %d", &cordRow, &cords[1], &rotation);

                cords[0] = cordRow - '0' - 17;
                cords[1] -= 1;

                if(CanPlace(MapPlayer,type,cords,rotation) == 1){
                    CreateShip(&FleetPlayer,type,cords,rotation,1);
                    done = 1;
                }
                else{
                    printf("\nOda nem rakhatod!\n");
                }
            }

        }
    }
    else{
        for(int i = 0; i < numberOfShips; i++){
            int done = 0;
            while(done == 0){
                done = RandomPlace(player,shipSet[i]);
            }
        }
    }
}

int StartGame(){
    Setup(0,1);
    printf("\nManuálisan vagy autómatikusan szeretnéd lerakni a hajóidat?( M/ A)\n");
    char gameType;
    int autoTurn = 0;

    scanf(" %c", &gameType);
    
    switch(gameType){
        case 'M':
            Setup(1,0);
            break;
        case 'A':
            Setup(1,1);
            break;
        case 'R':
            Setup(1,1);
            autoTurn = 1;
            break;
        default:
            Setup(1,1);
            break;
    }

    DrawMap(0,0);
    return autoTurn;

}

int IsSunk(struct Ship ship){
    for(int i = 0; i < ship.SegmentCount; i++){
        if(ship.Segments[i]->Shot == 0){
            return 0;
        }
    }
    return 1;
}

void ShootAround(struct Segment map[10][10], int cords[2], int player){
    int possible[8][2] = {
        {0,1},  //0 = right
        {0,-1}, //1 = left
        {1,0},  //2 = down
        {-1,0}, //3 = up
        {1,1},  //4 = down-right
        {1,-1}, //5 = down-left
        {-1,-1},//6 = up-left
        {-1,1}  //7 = up-right
    };

    for(int i = 0; i < 8; i++){
        int tempcord[2] = {possible[i][0] + cords[0],possible[i][1] + cords[1]};
        if(tempcord[0] > -1 && tempcord[0] < 10 && tempcord[1] > -1 && tempcord[1] < 10 && map[tempcord[0]][tempcord[1]].Shot == 0){
            map[tempcord[0]][tempcord[1]].Shot = 1;
            if(map[tempcord[0]][tempcord[1]].ShipID[0] != '-'){
                if(player == 1){
                    Hit(&FleetEnemy,map[tempcord[0]][tempcord[1]].ShipID,player);
                }
                else{
                    Hit(&FleetPlayer,map[tempcord[0]][tempcord[1]].ShipID,player);
                }
            }
        }
    }
}

void AreaReveal(struct Segment map[10][10], struct Ship ship, int player){
    for(int i = 0; i < ship.SegmentCount; i++){
        int segmentCord[2] = {ship.Coordinates[0] + ship.Segments[i]->RelativeCoordinates[0],ship.Coordinates[1] + ship.Segments[i]->RelativeCoordinates[1]};
        ShootAround(map,segmentCord,player);
    }
}

void Hit(struct Fleet *fleet, char id[2], int player){
    for(int i; i < fleet->ShipCount; i++){
        if(fleet->Ships[i].ID[0] == id[0]){
            if(IsSunk(fleet->Ships[i])){
                fleet->Ships[i].Sunk = 1;
                if(player == 1){
                    AreaReveal(MapEnemy,fleet->Ships[i],player);
                }
                else{
                    AreaReveal(MapPlayer,fleet->Ships[i],player);
                }
                DrawMap(0,0);
                printf("\nTalált, sűlyedt!\n");
            }
            return;
        }
    }
}

int Shoot(struct Segment map[10][10], int cords[2], int player, int autoTurn){
    if(cords[0] > -1 && cords[0] < 10 && cords[1] > -1 && cords[1] < 10){
        if(map[cords[0]][cords[1]].Shot == 0){
            map[cords[0]][cords[1]].Shot = 1;
            DrawMap(0,0);
            if(map[cords[0]][cords[1]].ShipID[0] != '-'){
                if(player == 1){    
                    printf("\nTalált!\n");
                    Hit(&FleetEnemy,map[cords[0]][cords[1]].ShipID,player);
                }
                else{
                    printf("\nAz ellenfél talált talált! (%c:%d)\n",cords[0] + 17 + '0',cords[1]);
                    Hit(&FleetPlayer,map[cords[0]][cords[1]].ShipID,player);
                }
                return 0;
            }
            if(player == 1){
                printf("\nNem talált\n");
            }
            else{
                printf("\nAz ellenfél nem talált talált! (%c:%d)\n",cords[0] + 17 + '0',cords[1]);
            }
            return 1;
        }
        else{
            DrawMap(0,0);
            if(player == 1 && autoTurn == 0){
                printf("\nOda már lőttél\n");
            }
            return 0;
        }    
    }
    DrawMap(0,0);
    printf("\nA koordináta nincs a táblán\n");
    return 0;
    
}

int CheckGameOver(struct Fleet toCheck){
    for(int i = 0; i < toCheck.ShipCount; i++){
        if(toCheck.Ships[i].Sunk == 0){
            return 0;
        }
    }
    return 1;
}

void Turn(int player, int autoTurn){
    int turnOver = 0;

    while(turnOver == 0){
        int cords[2];
        if(player == 1){
            if(autoTurn == 0){
                char cordRow;
                

                printf("\nHová szeretnél lőni?\n A-J:1-10\n");
                scanf(" %c:%d", &cordRow, &cords[1]);

                cords[0] = cordRow - '0' - 17;
                cords[1] -= 1;

                turnOver = Shoot(MapEnemy,cords,1,0);
                if(CheckGameOver(FleetPlayer) || CheckGameOver(FleetEnemy)){
                    turnOver = 1;
                }
            }
            else{
                cords[0] = rand() % 10;
                cords[1] = rand() % 10;
                turnOver = Shoot(MapEnemy,cords,1,1);
                if(CheckGameOver(FleetPlayer) || CheckGameOver(FleetEnemy)){
                    turnOver = 1;
                }
                if(turnOver == 0){
                    usleep(1500);
                }
            }
        }
        else{
            cords[0] = rand() % 10;
            cords[1] = rand() % 10;
            turnOver = Shoot(MapPlayer,cords,0,0);
            if(CheckGameOver(FleetPlayer) || CheckGameOver(FleetEnemy)){
                turnOver = 1;
            }
            if(turnOver == 0){
                usleep(1500);
            }
        }
    }
}

int Game(int autoTurn){
    while(1){
        if(autoTurn == 1){
            usleep(1500);
        }
        Turn(1, autoTurn);
        if(CheckGameOver(FleetEnemy)){
            return 1;
        }
        usleep(1500);
        Turn(0,0);
        if(CheckGameOver(FleetPlayer)){
            return 0;
        }
    }
}

void main(){

    srand(time(NULL)); //randomhoz kell

    struct Segment Empty = {
        {'-','-'},
        0,
        {0,0}
    };

    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            MapPlayer[i][j] = Empty;
            MapEnemy[i][j] = Empty;
        }
    }
    
    int autoTurn = StartGame();

    if(Game(autoTurn)){
        printf("\nNyertél! :D\n");
    }
    else{
        DrawMap(0,1);
        printf("\nVesztettél :(\n");
    }

    printf("\nEOC\n");
}