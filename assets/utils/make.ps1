Clear-Host
Clear-Host
Write-Host --------------------------------------
Write-Host    Make paconnor....
Write-Host --------------------------------------
$parentPath = Split-Path -Parent $PSScriptRoot
Write-Host Cleaning old files....
Remove-Item "$parentPath/binaries/*.DAT"
Remove-Item "$parentPath/fonts/*.DAT"
Remove-Item "$parentPath/images/*.DAT"
Remove-Item "$parentPath/maps/*.DAT"
Remove-Item "$parentPath/palettes/*.DAT"
Remove-Item "$parentPath/sounds/*.DAT"
Remove-Item "$parentPath/sprites/*.DAT"
Remove-Item "$parentPath/tilesets/*.DAT"
Remove-Item "$parentPath/utils/*.DAT"
Remove-Item "$parentPath/text/ES/*.DAT"
Remove-Item "$parentPath/text/EN/*.DAT"
Remove-Item "$parentPath/text/FR/*.DAT"

Write-Host ////////////////////////////// BINARY FILES /////////////////////////////////
Write-Host Generating binary DAT file....
Set-Location -Path "$parentPath\binaries"
python3 "$PSScriptRoot/makedat.py" "BINARIES.DAT" "ENGINE.BIN" "ERROR.BIN" "NOMEMORY.BIN" "NOVIDEO.BIN" "SHUTDOWN.BIN"

Write-Host ////////////////////////////// FONTS /////////////////////////////////
Write-Host Generating font DAT file....
Set-Location -Path "$parentPath\fonts"
python3 "$PSScriptRoot/makedat.py" "FONTS.DAT" "SLIMW.PCX" "SLIMB.PCX" "SLIMY.PCX" "BIGW.PCX" "BIGB.PCX"

Write-Host ////////////////////////////// IMAGES /////////////////////////////////
Write-Host Generating logo images DAT file....
Set-Location -Path "$parentPath\images\logo"
python3 "$PSScriptRoot/makedat.py" "ILOGO.DAT" "LCONTEST.PCX" "LDOSCLUB.PCX" "LMANOL.PCX" "JOY1.PCX" "JOY2.PCX" "JOY3.PCX"
Write-Host Generating menu images DAT file....
Set-Location -Path "$parentPath\images\menu"
python3 "$PSScriptRoot/makedat.py" "IMENU.DAT" "MENUBACK.PCX"
Write-Host Generating intro images DAT file....
Set-Location -Path "$parentPath\images\intro"
python3 "$PSScriptRoot/makedat.py" "INTRO.DAT" "INTRO1.PCX" "INTRO2.PCX" "INTRO3.PCX"
Write-Host Generating end images DAT file....
Set-Location -Path "$parentPath\images\end"
python3 "$PSScriptRoot/makedat.py" "END.DAT" "END1.PCX"

Write-Host ////////////////////////////// SCENE IMAGES /////////////////////////////////
Write-Host Generating scene 1 intro images DAT file....
Set-Location -Path "$parentPath\images\scn1"
python3 "$PSScriptRoot/makedat.py" "ISCN1.DAT" "INTRO0.PCX" "INTRO1H.PCX" "INTRO1L.PCX" "INTRO2H.PCX" "INTRO2L.PCX" "INTRO3H.PCX" "INTRO4H.PCX" "INTRO4L.PCX" "INTRO5H.PCX" "INTRO5L.PCX" "INTRO6H.PCX" "INTRO7H.PCX" "INTRO7L.PCX" "INTRO8H.PCX" "INTRO8L.PCX" "INTRO9H.PCX" "INTRO9L.PCX" "INTRO10H.PCX" 
Write-Host Generating scene 1 outro images DAT file....
Set-Location -Path "$parentPath\images\scn1"
python3 "$PSScriptRoot/makedat.py" "OSCN1.DAT" "OUTRO1H.PCX" "OUTRO2H.PCX" "OUTRO2L.PCX" "OUTRO3H.PCX" "OUTRO3L.PCX" "OUTRO4H.PCX" "OUTRO5H.PCX" "OUTRO5L.PCX" "OUTRO6H.PCX" "OUTRO7H.PCX" "OUTRO8H.PCX"
Write-Host Generating scene 2 intro images DAT file....
Set-Location -Path "$parentPath\images\scn2"
python3 "$PSScriptRoot/makedat.py" "ISCN2.DAT" "INTRO1H.PCX" "INTRO2H.PCX" "INTRO3H.PCX" 
Write-Host Generating scene 2 outro images DAT file....
Set-Location -Path "$parentPath\images\scn2"
python3 "$PSScriptRoot/makedat.py" "OSCN2.DAT" "OUTRO1H.PCX" "OUTRO1L.PCX"
Write-Host Generating scene 3 intro images DAT file....
Set-Location -Path "$parentPath\images\scn3"
python3 "$PSScriptRoot/makedat.py" "ISCN3.DAT" "INTRO1H.PCX" "INTRO1L.PCX"
Write-Host Generating scene 3 outro images DAT file....
Set-Location -Path "$parentPath\images\scn3"
python3 "$PSScriptRoot/makedat.py" "OSCN3.DAT" "OUTRO1H.PCX" "OUTRO1L.PCX"
Write-Host Generating scene 4 intro images DAT file....
Set-Location -Path "$parentPath\images\scn4"
python3 "$PSScriptRoot/makedat.py" "ISCN4.DAT" "INTRO1H.PCX" "INTRO1L.PCX"
Write-Host Generating scene 4 outro images DAT file....
Set-Location -Path "$parentPath\images\scn4"
python3 "$PSScriptRoot/makedat.py" "OSCN4.DAT" "OUTRO1H.PCX" "OUTRO1L.PCX"
Write-Host Generating scene 5 intro images DAT file....
Set-Location -Path "$parentPath\images\scn5"
python3 "$PSScriptRoot/makedat.py" "ISCN5.DAT" "INTRO1H.PCX" "INTRO1L.PCX"
Write-Host Generating scene 5 outro images DAT file....
Set-Location -Path "$parentPath\images\scn5"
python3 "$PSScriptRoot/makedat.py" "OSCN5.DAT" "OUTRO1H.PCX" "OUTRO1L.PCX"
Write-Host Generating scene 6 intro images DAT file....
Set-Location -Path "$parentPath\images\scn6"
python3 "$PSScriptRoot/makedat.py" "ISCN6.DAT" "INTRO1H.PCX" "INTRO1L.PCX"
Write-Host Generating scene 6 outro images DAT file....
Set-Location -Path "$parentPath\images\scn6"
python3 "$PSScriptRoot/makedat.py" "OSCN6.DAT" "OUTRO1H.PCX" "OUTRO1L.PCX"

Write-Host ////////////////////////////// MAPS /////////////////////////////////
Write-Host Generating scene 1-1 map DAT file....
Set-Location -Path "$parentPath\maps\scn1_1"
python3 "$PSScriptRoot/makedat.py" "MAPSCN11.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 1-2 map DAT file....
Set-Location -Path "$parentPath\maps\scn1_2"
python3 "$PSScriptRoot/makedat.py" "MAPSCN12.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 2-1 map DAT file....
Set-Location -Path "$parentPath\maps\scn2_1"
python3 "$PSScriptRoot/makedat.py" "MAPSCN21.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 2-2 map DAT file....
Set-Location -Path "$parentPath\maps\scn2_2"
python3 "$PSScriptRoot/makedat.py" "MAPSCN22.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 3-1 map DAT file.... 
Set-Location -Path "$parentPath\maps\scn3_1"
python3 "$PSScriptRoot/makedat.py" "MAPSCN31.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 3-2 map DAT file....
Set-Location -Path "$parentPath\maps\scn3_2"
python3 "$PSScriptRoot/makedat.py" "MAPSCN32.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 4-1 map DAT file.... 
Set-Location -Path "$parentPath\maps\scn4_1"
python3 "$PSScriptRoot/makedat.py" "MAPSCN41.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 4-2 map DAT file....
Set-Location -Path "$parentPath\maps\scn4_2"
python3 "$PSScriptRoot/makedat.py" "MAPSCN42.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 5-1 map DAT file....
Set-Location -Path "$parentPath\maps\scn5_1"
python3 "$PSScriptRoot/makedat.py" "MAPSCN51.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 5-2 map DAT file....
Set-Location -Path "$parentPath\maps\scn5_2"
python3 "$PSScriptRoot/makedat.py" "MAPSCN52.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 6-1 map DAT file....
Set-Location -Path "$parentPath\maps\scn6_1"
python3 "$PSScriptRoot/makedat.py" "MAPSCN61.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"
Write-Host Generating scene 6-2 map DAT file....
Set-Location -Path "$parentPath\maps\scn6_2"
python3 "$PSScriptRoot/makedat.py" "MAPSCN62.DAT" "m_back.csv" "m_fore.csv" "m_mask.csv" "m_col.csv" "m_anim.csv" "m_event.csv" "m_hspot.csv"

Write-Host ////////////////////////////// PALETTES /////////////////////////////////
Write-Host Generating palettes DAT file....
Set-Location -Path "$parentPath\palettes"
python3 "$PSScriptRoot/makedat.py" "PALETTES.DAT" "LDOSCLUB.PCX" "LMANOL.PCX" "LCONTEST.PCX" "INTRO.PCX" "MENU.PCX" "LOADING.PCX" "ISCN1.PCX" "SCN11.PCX" "SCN12.PCX" "ISCN2.PCX" "SCN21.PCX" "SCN22.PCX" "SCN31.PCX" "SCN32.PCX" "SCN41.PCX" "SCN42.PCX" "SCN51.PCX" "SCN52.PCX" "SCN61.PCX" "SCN62.PCX" "END.PCX"

Write-Host ////////////////////////////// SOUND /////////////////////////////////
Write-Host Generating music DAT file....
Set-Location -Path "$parentPath\sounds"
python3 "$PSScriptRoot/makedat.py" "MUSICSPK.DAT" "spk_song1.mus" "spk_song2.mus" "spk_song3.mus" "spk_song4.mus" "spk_song5.mus" "spk_song6.mus" "spk_song7.mus"
python3 "$PSScriptRoot/makedat.py" "MSCENE1.DAT" "intro1.a2m" "scene1.a2m"
python3 "$PSScriptRoot/makedat.py" "MSCENE2.DAT" "intro2.a2m" "scene2.a2m"

Write-Host ////////////////////////////// CURSOR /////////////////////////////////
Write-Host Generating cursor sprites DAT file....
Set-Location -Path "$parentPath\sprites\cursor"
python3 "$PSScriptRoot/makedat.py" "CURSOR.DAT" "AIM.PCX" "POINTER.PCX"

Write-Host ////////////////////////////// SPRITES /////////////////////////////////
Write-Host Generating intro sprites DAT file....
Set-Location -Path "$parentPath\sprites\intro"
python3 "$PSScriptRoot/makedat.py" "ISPR.DAT" "ACTOR.PCX"
Write-Host Generating actor sprites DAT file....
Set-Location -Path "$parentPath\sprites\actor"
python3 "$PSScriptRoot/makedat.py" "ACTOR.DAT" "FEET.PCX" "BODY.PCX" "HEAD.PCX" "LARM1.PCX" "RARM1.PCX" "RARM2.PCX" "RARM3.PCX" "RARM4.PCX" "RARM5.PCX" "FEET.ANI" "BODY.ANI" "HEAD.ANI" "LARM.ANI" "RARM.ANI" "FACE.PCX" "PORTAIT.PCX"
Write-Host Generating actor animation DAT file....
Set-Location -Path "$parentPath\sprites\actor"
python3 "$PSScriptRoot/makedat.py" "ACTANI.DAT" "FEET.ANI" "BODY.ANI" "HEAD.ANI" "RARM.ANI" "LARM.ANI"
Write-Host Generating bullet sprites DAT file....
Set-Location -Path "$parentPath\sprites\bullets"
python3 "$PSScriptRoot/makedat.py" "BULLETS.DAT" "BULLET1.PCX" "BULLET2.PCX" "PUKE.PCX" "PUNCH1.PCX" "GRENADE1.PCX" "GRENADES.PCX"
Write-Host Generating enemy 1 sprites DAT file....
Set-Location -Path "$parentPath\sprites\enemies\enemy1"
python3 "$PSScriptRoot/makedat.py" "ENEMY1.DAT" "FACE.PCX" "PORTAIT.PCX" "FEET.PCX" "BODY.PCX" "HEAD.PCX" "LARM.PCX" "RARM.PCX"
Write-Host Generating enemy 2 sprites DAT file....
Set-Location -Path "$parentPath\sprites\enemies\enemy2"
python3 "$PSScriptRoot/makedat.py" "ENEMY2.DAT" "FACE.PCX" "PORTAIT.PCX" "FEET.PCX" "BODY.PCX" "HEAD.PCX" "LARM.PCX" "RARM.PCX"
Write-Host Generating enemy 2 sprites DAT file....
Set-Location -Path "$parentPath\sprites\enemies\enemy2"
python3 "$PSScriptRoot/makedat.py" "ENEMY2.DAT" "FACE.PCX" "PORTAIT.PCX" "FEET.PCX" "BODY.PCX" "HEAD.PCX" "LARM.PCX" "RARM.PCX"
Write-Host Generating enemy 3 sprites DAT file....
Set-Location -Path "$parentPath\sprites\enemies\enemy3"
python3 "$PSScriptRoot/makedat.py" "ENEMY3.DAT" "FACE.PCX" "PORTAIT.PCX" "FEET.PCX" "BODY.PCX" "HEAD.PCX" "LARM.PCX" "RARM.PCX"
Write-Host Generating enemy 4 sprites DAT file....
Set-Location -Path "$parentPath\sprites\enemies\enemy4"
python3 "$PSScriptRoot/makedat.py" "ENEMY4.DAT" "FACE.PCX" "PORTAIT.PCX" "FEET.PCX" "BODY.PCX" "HEAD.PCX" "LARM.PCX" "RARM.PCX"
Write-Host Generating enemy 5 sprites DAT file....
Set-Location -Path "$parentPath\sprites\enemies\enemy5"
python3 "$PSScriptRoot/makedat.py" "ENEMY5.DAT" "FACE.PCX" "PORTAIT.PCX" "FEET.PCX" "BODY.PCX" "HEAD.PCX" "LARM.PCX" "RARM.PCX"
Write-Host Generating enemy 6 sprites DAT file....
Set-Location -Path "$parentPath\sprites\enemies\enemy6"
python3 "$PSScriptRoot/makedat.py" "ENEMY6.DAT" "FACE.PCX" "PORTAIT.PCX" "FEET.PCX" "BODY.PCX" "HEAD.PCX" "LARM.PCX" "RARM.PCX"
Write-Host Generating boss1 sprites DAT file....
Set-Location -Path "$parentPath\sprites\boss\boss1"
python3 "$PSScriptRoot/makedat.py" "BOSS1.DAT" "FACE.PCX" "PORTAIT.PCX" "FEET.PCX" "BODY.PCX" "HEAD.PCX" "LARM.PCX" "RARM.PCX"
Write-Host Generating boss animation DAT file....
Set-Location -Path "$parentPath\sprites\boss\anim"
python3 "$PSScriptRoot/makedat.py" "BOSSANI.DAT" "FEET.ANI" "BODY.ANI" "HEAD.ANI" "RARM.ANI" "LARM.ANI"
Write-Host Generating npcs DAT file....
Set-Location -Path "$parentPath\sprites\npcs"
python3 "$PSScriptRoot/makedat.py" "NPCS.DAT" "SARA.PCX" "SARAF.PCX" "DROID1.PCX" "DROID1F.PCX" "DROID2.PCX" "DROID2F.PCX" "DROID3.PCX" "DROID3F.PCX" "AGENT1.PCX" "AGENT1F.PCX" "AGENT2.PCX" "AGENT2F.PCX" "BOSS.PCX" "BOSSF.PCX"
Write-Host Generating misc sprites DAT file....
Set-Location -Path "$parentPath\sprites\misc"
python3 "$PSScriptRoot/makedat.py" "MISC.DAT" "BARREL1.PCX" "BARREL1P.PCX" "IWORMS.PCX" "CHAT.PCX" "CHAT_BIG.PCX" "FUCK.PCX" "EMPTY.PCX" "TARGET1.PCX" "TARGET2.PCX"
Write-Host Generating effects sprites DAT file....
Set-Location -Path "$parentPath\sprites\effects"
python3 "$PSScriptRoot/makedat.py" "EFFECTS.DAT" "FIRE1.PCX" "ELECTRIC1.PCX" "SPARK.PCX" "PUNCHL.PCX" "PISTOLL.PCX" "SHOTGUNL.PCX" "UZIL.PCX" "SNIPPERL.PCX" "BLOOD.PCX" "EXPLOSION.PCX" "TIMER.PCX" "CLICK.PCX" "RAIN.PCX"
Write-Host Generating panel sprites DAT file....
Set-Location -Path "$parentPath\sprites\panel"
python3 "$PSScriptRoot/makedat.py" "PANEL.DAT" "LIFEBAR.PCX" "CHAT.PCX" "GUN00.PCX" "GUN01.PCX" "GUN02.PCX" "GUN03.PCX" "GUN04.PCX" "GUN05.PCX" "GRENADE.PCX"
Write-Host Generating items sprites DAT file....
Set-Location -Path "$parentPath\sprites\items"
python3 "$PSScriptRoot/makedat.py" "ITEMS.DAT" "MEDIKIT.PCX" "AMMO1.PCX" "AMMO2.PCX" "AMMO3.PCX" "AMMO4.PCX" "GRENADE.PCX"
Write-Host Generating button sprites DAT file....
Set-Location -Path "$parentPath\sprites\menu"
python3 "$PSScriptRoot/makedat.py" "SMENU.DAT" "BTNNEW.PCX" "BTNPASS.PCX" "BTNOPT.PCX" "BTNCRED.PCX" "BTNEXIT.PCX" "BTNBACK.PCX" "BTNOK.PCX" "BTNLEFT.PCX" "BTNRIGHT.PCX" "MCHAT.PCX" "POPTIONS.PCX" "PPASS.PCX" "MTITLE.PCX"

Write-Host ////////////////////////////// ANIMATIONS /////////////////////////////////
Write-Host Generating generic enemies animation DAT file....
Set-Location -Path "$parentPath\sprites\enemies\anim_gen"
python3 "$PSScriptRoot/makedat.py" "ENEMANI.DAT" "FEET.ANI" "BODY.ANI" "HEAD.ANI" "RARM.ANI" "LARM.ANI"
Write-Host Generating rat animation DAT file....
Set-Location -Path "$parentPath\sprites\enemies\anim_rat"
python3 "$PSScriptRoot/makedat.py" "RATANI.DAT" "FEET.ANI" "BODY.ANI" "HEAD.ANI" "RARM.ANI" "LARM.ANI"

Write-Host ////////////////////////////// TILESETS /////////////////////////////////
Write-Host Generating scene 1-1 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn1_1"
python3 "$PSScriptRoot/makedat.py" "TSCN11.DAT" "scn1_1_back.pcx" "scn1_1_fore.pcx" "scn1_1_mask.pcx" 
Write-Host Generating scene 1-2 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn1_2"
python3 "$PSScriptRoot/makedat.py" "TSCN12.DAT" "scn1_2_back.pcx" "scn1_2_fore.pcx" "scn1_2_mask.pcx" 
Write-Host Generating scene 2-1 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn2_1"
python3 "$PSScriptRoot/makedat.py" "TSCN21.DAT" "scn2_1_back.pcx" "scn2_1_fore.pcx" "scn2_1_mask.pcx" 
Write-Host Generating scene 2-2 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn2_2"
python3 "$PSScriptRoot/makedat.py" "TSCN22.DAT" "scn2_2_back.pcx" "scn2_2_fore.pcx" "scn2_2_mask.pcx" 
Write-Host Generating scene 3-1 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn3_1"
python3 "$PSScriptRoot/makedat.py" "TSCN31.DAT" "scn3_1_back.pcx" "scn3_1_fore.pcx" "scn3_1_mask.pcx" 
Write-Host Generating scene 3-2 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn3_2"
python3 "$PSScriptRoot/makedat.py" "TSCN32.DAT" "scn3_2_back.pcx" "scn3_2_fore.pcx" "scn3_2_mask.pcx" 
Write-Host Generating scene 4-1 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn4_1"
python3 "$PSScriptRoot/makedat.py" "TSCN41.DAT" "scn4_1_back.pcx" "scn4_1_fore.pcx" "scn4_1_mask.pcx" 
Write-Host Generating scene 4-2 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn4_2"
python3 "$PSScriptRoot/makedat.py" "TSCN42.DAT" "scn4_2_back.pcx" "scn4_2_fore.pcx" "scn4_2_mask.pcx" 
Write-Host Generating scene 5-1 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn5_1"
python3 "$PSScriptRoot/makedat.py" "TSCN51.DAT" "scn5_1_back.pcx" "scn5_1_fore.pcx" "scn5_1_mask.pcx" 
Write-Host Generating scene 5-2 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn5_2"
python3 "$PSScriptRoot/makedat.py" "TSCN52.DAT" "scn5_2_back.pcx" "scn5_2_fore.pcx" "scn5_2_mask.pcx" 
Write-Host Generating scene 6-1 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn6_1"
python3 "$PSScriptRoot/makedat.py" "TSCN61.DAT" "scn6_1_back.pcx" "scn6_1_fore.pcx" "scn6_1_mask.pcx" 
Write-Host Generating scene 6-2 tileset DAT file....
Set-Location -Path "$parentPath\tilesets\scn6_2"
python3 "$PSScriptRoot/makedat.py" "TSCN62.DAT" "scn6_2_back.pcx" "scn6_2_fore.pcx" "scn6_2_mask.pcx" 

Write-Host ////////////////////////////// DIALOGS /////////////////////////////////
write-Host Generating spanish text dialog DAT file....
Set-Location -Path "$parentPath\text\ES"
python3 "$PSScriptRoot/makedat.py" "DIALOGES.DAT" "INTRO.TXT" "NAMES.TXT" "GLOBAL.TXT" "CREDITS.TXT" "SCN1I.TXT" "SCN1H.TXT" "SCN1D.TXT" "SCN2I.TXT" "SCN2H.TXT" "SCN2D.TXT" "SCN3I.TXT" "SCN3H.TXT" "SCN3D.TXT" "SCN4I.TXT" "SCN4H.TXT" "SCN4D.TXT" "SCN5I.TXT" "SCN5H.TXT" "SCN5D.TXT" "SCN6I.TXT" "SCN6H.TXT" "SCN6D.TXT" "SCN7D.TXT"
write-Host Generating english text dialog DAT file....
Set-Location -Path "$parentPath\text\EN"
python3 "$PSScriptRoot/makedat.py" "DIALOGEN.DAT" "INTRO.TXT" "NAMES.TXT" "GLOBAL.TXT" "CREDITS.TXT" "SCN1I.TXT" "SCN1H.TXT" "SCN1D.TXT" "SCN2I.TXT" "SCN2H.TXT" "SCN2D.TXT" "SCN3I.TXT" "SCN3H.TXT" "SCN3D.TXT" "SCN4I.TXT" "SCN4H.TXT" "SCN4D.TXT" "SCN5I.TXT" "SCN5H.TXT" "SCN5D.TXT" "SCN6I.TXT" "SCN6H.TXT" "SCN6D.TXT" "SCN7D.TXT" 
write-Host Generating french text dialog DAT file....
Set-Location -Path "$parentPath\text\FR"
python3 "$PSScriptRoot/makedat.py" "DIALOGFR.DAT" "INTRO.TXT" "NAMES.TXT" "GLOBAL.TXT" "CREDITS.TXT" "SCN1I.TXT" "SCN1H.TXT" "SCN1D.TXT" "SCN2I.TXT" "SCN2H.TXT" "SCN2D.TXT" "SCN3I.TXT" "SCN3H.TXT" "SCN3D.TXT" "SCN4I.TXT" "SCN4H.TXT" "SCN4D.TXT" "SCN5I.TXT" "SCN5H.TXT" "SCN5D.TXT" "SCN6I.TXT" "SCN6H.TXT" "SCN6D.TXT" "SCN7D.TXT"


write-Host Generating animation DAT file....
Write-Host --------------------------------------
Write-Host    Make Process end 
Write-Host --------------------------------------
Write-Host --------------------------------------
Write-Host    Copying dat files to build folder
Write-Host --------------------------------------
Start-Sleep -Seconds 3

