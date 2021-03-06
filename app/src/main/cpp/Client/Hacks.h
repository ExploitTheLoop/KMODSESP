#ifndef HACKS_H
#define HACKS_H

#include "kmods.h"

bool isValidPlayer(PlayerData data){
    return (data.Body != Vector2::Zero() && data.Head != Vector2::Zero());
}

bool isValidItem(ItemData data){
    return (data.Location != Vector2::Zero());
}

/*int isOutsideSafezone(Vector2 pos, Vector2 screen) {
    Vector2 mSafezoneTopLeft(screen.x * 0.04f, screen.y * 0.04f);
    Vector2 mSafezoneBottomRight(screen.x * 0.96f, screen.y * 0.96f);

    int result = 0;
    if (pos.y < mSafezoneTopLeft.y) {
        // top
        result |= 1;
    }
    if (pos.x > mSafezoneBottomRight.x) {
        // right
        result |= 2;
    }
    if (pos.y > mSafezoneBottomRight.y) {
        // bottom
        result |= 4;
    }
    if (pos.x < mSafezoneTopLeft.x) {
        // left
        result |= 8;
    }
    return result;
}*/

Vector2 pushToScreenBorder(Vector2 Pos, Vector2 screen, int offset) {
    int x = (int)Pos.x;
    int y = (int)Pos.y;
    if (Pos.y < 0) {
        // top
        y = -offset;
    }
    if (Pos.x > screen.x) {
        // right
        x = (int)screen.x + offset;
    }
    if (Pos.y > screen.y) {
        // bottom
        y = (int)screen.y + offset;
    }
    if (Pos.x < 0) {
        // left
        x = -offset;
    }
    return Vector2(x, y);
}

bool isOutsideSafeZone(Vector2 pos, Vector2 screen) {
    if (pos.y < 0) {
        return true;
    }
    if (pos.x > screen.x) {
        return true;
    }
    if (pos.y > screen.y) {
        return true;
    }
    return pos.x < 0;
}

int count_real_players(Response response){
    int t = 0;

    for(int i=0; i<response.PlayerCount; i++){
        if(!response.Players[i].isBot){
            t = t+1;
        }
    }

    return t;
}

bool isInCenter(Rect box,int screenWidth, int screenHeight){
    Vector2 centerPos = Vector2(screenWidth / 2, screenHeight / 2);
    bool  w = false;
    if(box.x < centerPos.x  && box.y < centerPos.y){
      w = true;
    }
    return w;
}


void DrawESP(ESP esp, int screenWidth, int screenHeight) {


    if(isESP){

        esp.DrawCrosshair(Color(0, 0, 0, 255), Vector2(screenWidth / 2, screenHeight / 2), 42);

        Vector2 screen(screenWidth, screenHeight);
        float mScale = screenHeight / (float) 1080;

        if(isAimbot){
            esp.DrawCircle(Color::Red(),2,Vector2(screenWidth/2,screenHeight/2),250);
        }


        Response response = getData(screenWidth, screenHeight);
        if(response.Success){
            if(isNearEnemy) {
              //  Color color = (response.NearEnemy > 0) ? Color::Red() : Color(0, 255, 55,200);
                Color clr2 = (response.NearEnemy > 0) ? Color::Red() : Color::Black();

                string enemyData = "CLEAR";
                if(response.NearEnemy >0){
                    enemyData = " ";
                    enemyData += to_string(response.NearEnemy);
                }

                string ua = "                    BlackHawk";

                esp.DrawText(Color::Yellow(), ua.c_str(), Vector2((screenWidth / 3)-650, 200), 20);
                esp.DrawSSNearEnemyBar(screenWidth, response.NearEnemy);
                esp.DrawText(clr2, enemyData.c_str(), Vector2((screenWidth / 2 - 10), 117), 27);


            }

            int count = response.PlayerCount;
            if(count > 0){
                for(int i=0; i < count; i++){
                    PlayerData player = response.Players[i];
                    if(!isValidPlayer(player)){ continue;}

                    bool isTeamMate = player.TeamID == response.MyTeamID;
                    if(isTeamMate && !isTeamMateShow){ continue;}
                    if(isIgnoreBots && player.isBot) { continue;}
                    Vector2 location = player.Body;
                    if(isPlayer360 && isOutsideSafeZone(location, screen)){
                        string dist;
                        dist += to_string((int) player.Distance);
                        dist += "m";

                        Vector2 hintDotRenderPos = pushToScreenBorder(location, screen,
                                                                      (int) ((mScale * 100) / 3));
                        Vector2 hintTextRenderPos = pushToScreenBorder(location, screen,
                                                                       -(int) ((mScale * 36)));

                        Color color = (player.Distance < 100) ? Color(255,0,0,255) : Color(255, 252, 77,200);
                        esp.DrawFilledCircle((isTeamMate ? Color(0,255,0,255) : color), hintDotRenderPos, (mScale * 100));
                        esp.DrawText(Color::White(), dist.c_str(), hintTextRenderPos, playerTextSize);
                        continue;
                    }

                    float boxHeight = fabsf(player.Root.y - player.Head.y);
                    float boxWidth = boxHeight * 0.56;
                    Rect Box(player.Head.x - (boxWidth / 2), player.Head.y, boxWidth, boxHeight);
                    bool playerInCenter = isInCenter(Box,screenWidth,screenHeight);

                    if(isPlayerSkel){
                        Color skclr = playerInCenter ? Color::Green() : Color(244,108,0);
                        esp.DrawFilledCircle(Color::White(), player.Head, boxWidth / 50);

                        esp.DrawLine(skclr, 5, player.Neck, player.Chest);
                        esp.DrawLine(skclr, 5, player.Chest, player.Pelvis);

                        esp.DrawLine(skclr, 5, player.Chest, player.LShoulder);
                        esp.DrawLine(skclr, 5, player.Chest, player.RShoulder);

                        esp.DrawLine(skclr, 5, player.LShoulder, player.LElbow);
                        esp.DrawLine(skclr, 5, player.RShoulder, player.RElbow);

                        esp.DrawLine(skclr, 5, player.LElbow, player.LWrist);
                        esp.DrawLine(skclr, 5, player.RElbow, player.RWrist);

                        esp.DrawLine(skclr, 5, player.Pelvis, player.LThigh);
                        esp.DrawLine(skclr, 5, player.Pelvis, player.RThigh);

                        esp.DrawLine(skclr, 5, player.LThigh, player.LKnee);
                        esp.DrawLine(skclr, 5, player.RThigh, player.RKnee);

                        esp.DrawLine(skclr, 5, player.LKnee, player.LAnkle);
                        esp.DrawLine(skclr, 5, player.RKnee, player.RAnkle);
                    }

                    if(isPlayerLine){
                        Color color = (player.isBot ? Color(255,255,255,200) : Color::Red());
                        int thck = (player.isBot? 1:2);
                        esp.DrawLine(color, thck, Vector2((screenWidth / 2), 0), player.Neck);
                    }

                    if(isPlayerBox ){
                        Color bxclr = playerInCenter ? Color::Green() : Color::Red();
                        esp.DrawRect(bxclr, 5, Box);
                    }


                    if(isPlayerHealth) {

                      /*  if(player.Distance < 80){
                            esp.DrawVerticalHealthBar(
                                    Vector2(Box.x + Box.width, Box.y),
                                    boxHeight,
                                    100, player.Health);
                        } else {
                            esp.DrawSSHealthBar(
                                    Vector2(Box.x - (110 * mScale), Box.y - 25),
                                    (200 * mScale),
                                    100, player.Health);

                        } */
                        esp.DrawSSHealthBar(
                                Vector2(Box.x - (110 * mScale), Box.y - 25),
                                (210 * mScale),
                                100, player.Health);
                    }

                    if(isPlayerName) {
                        wstring pname = player.PlayerName;
                        if(player.isBot){
                            pname += L" [Bot]";
                        }
                        pname += L"  ";
                        pname += to_wstring(player.TeamID);
                        pname += L" ";
                        Rect rect = Rect(Box.x-90 ,Box.y-33,205,30);

                        //esp.DrawLineBorder(Color::White(),1,rect);
                        //esp.DrawFilledRect(Color(0,0,0,200),rect);
                        //esp.DrawFilledRect(Color(0,255,0,90),rect);
                        esp.DrawPlayerText(Color::White(), pname.c_str(),
                                           Vector2(Box.x + (Box.width / 2), Box.y - 12),
                                           playerTextSize);
                    }

                    if(isPlayerDist) {
                        string dist;
                        dist += "[ ";
                        dist += to_string((int) player.Distance);
                        dist += "m";
                        dist += " ]";

                        esp.DrawText(Color::Yellow(), dist.c_str(),
                                     Vector2(Box.x + (Box.width / 2),
                                             Box.y + Box.height + 25), playerTextSize);
                    }


                }
            }

            count = response.ItemsCount;
            if(count > 0){
                for(int i=0; i < count; i++){
                    ItemData item = response.Items[i];
                    if(!isValidItem(item)){ continue;}

                    if(item.isVehicle && !isVehicle){
                        continue;
                    } else if(item.isLootBox && !isLootBox){
                        continue;
                    } else if(item.isAirDrop && !isAirDrop){
                        continue;
                    } else if(item.isLootItem && !isLootItems){
                        continue;
                    }

                    Vector2 location = item.Location;

                    string dist;
                    dist += "[ ";
                    dist += to_string((int) item.Distance);
                    dist += "M";
                    dist += " ]";

                    if(isItemName) {
                        esp.DrawText(Color::Pink(), item.Name, Vector2(
                                location.x, location.y + (20 * mScale)), itemTextSize);
                    }
                    if(isItemDist) {
                        esp.DrawText(Color(255,255,255,150), dist.c_str(), Vector2(
                                location.x, location.y + (40 * mScale)), itemTextSize);
                    }
                }
            }
        }
    }
}

void MemoryHacker(SocketClient client,int screenWidth, int screenHeight){

    if(isBypass){
        if(!isBypassActive) {
            Request request{MemMode, screenWidth, screenHeight, 0};
            client.send((void *) &request, sizeof(request));
            isBypassActive = true;
        }
    } else {
        isBypassActive = false;
    }


    if(isAimbot){
        if(!isAimbotActive) {
            Request request{MemMode, screenWidth, screenHeight, 1};
            client.send((void *) &request, sizeof(request));
            isAimbotActive = true;
        }
    } else {
        isAimbotActive = false;
    }
    if(isRecoil){
        if(!isRecoilActive) {
            Request request{MemMode, screenWidth, screenHeight, 2};
            client.send((void *) &request, sizeof(request));
            isRecoilActive = true;
        }
    }  else {
        isRecoilActive = false;
    }

    if(isNofog){
        if(!isNFActive) {
            Request request{MemMode, screenWidth, screenHeight, 3};
            client.send((void *) &request, sizeof(request));
            isNFActive = true;
        }
    }  else {
        isNFActive = false;
    }

    if(isBlacksky){
        if(!isBSActive) {
            Request request{MemMode, screenWidth, screenHeight, 4};
            client.send((void *) &request, sizeof(request));
            isBSActive = true;
        }
    }  else {
        isBSActive = false;
    }

    if(isHeadshot){
        if(!isHeadshotActive) {
            Request request{MemMode, screenWidth, screenHeight, 5};
            client.send((void *) &request, sizeof(request));
            isHeadshotActive = true;
        }

    }  else {
        isHeadshotActive = false;
    }




}
#endif //HACKS_H
