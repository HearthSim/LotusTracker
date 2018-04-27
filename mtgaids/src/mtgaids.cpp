#include "mtgaids.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonValueRef>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>

MtgaIDs::MtgaIDs(QObject *parent) : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    setsDir = dataDir + QDir::separator() + "sets";
    if(!QFile::exists(setsDir)) {
        QDir dir;
        dir.mkpath(setsDir);
    }
}

void MtgaIDs::process()
{
    loadSet("OGW", 62165, {});
    loadSet("AKH", 64801, getAKHUnorderedCardsId());
    loadSet("HOU", 65479, getHOUUnorderedCardsId());
    loadSet("XLN", 65961, getXLNUnorderedCardsId());
    loadSet("RIX", 66619, {});
    loadSet("DOM", 67106, getDOMUnorderedCardsId());
}

void MtgaIDs::loadSet(QString setCode, int firstCardId, QMap<QString, int> unorderedCardsId)
{
    setsFirstCardId[setCode] = firstCardId;
    setsUnorderedCardIds[setCode] = unorderedCardsId;
    QFile setFile(setsDir + QDir::separator() + setCode + ".json");
    if(QFileInfo(setFile).exists()) {
          loadSetFromFile(setCode + ".json");
    } else {
          downloadSet(setCode);
    }
}

void MtgaIDs::downloadSet(QString setCode)
{
    QString setUrl = QString("https://mtgjson.com/json/%1.json").arg(setCode);
    QNetworkRequest request(setUrl);
    QNetworkReply *reply = networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &MtgaIDs::downloadSetOnFinish);
    LOG(QString("Downloading %1 cards from %2").arg(setCode).arg(setUrl));
}

void MtgaIDs::downloadSetOnFinish()
{
  	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
  	QByteArray jsonData = reply->readAll();

  	if(reply->error() == QNetworkReply::ContentNotFoundError) {
        QString setUrl = reply->url().toString();
        LOG(QString("Error while downloading mtg card json: %1").arg(setUrl));
        return;
  	}

    QJsonParseError error;
    QJsonObject jsonSet = QJsonDocument::fromJson(jsonData, &error).object();
    QString setCode = jsonSet["code"].toString();
    LOG(QString("Downloaded %1 bytes from %2 json").arg(jsonData.size()).arg(setCode));

    QFile setFile(setsDir + QDir::separator() + setCode + ".json");
    setFile.open(QIODevice::WriteOnly);
    setFile.write(jsonData);
    setFile.close();

    loadSetFromFile(setCode + ".json");
}

void MtgaIDs::loadSetFromFile(QString setFileName) {
    LOG(QString("Loading %1").arg(setFileName));

    QFile setFile(setsDir + QDir::separator() + setFileName);
    if(!QFileInfo(setFile).exists()) {
        LOG(QString("%1 not found.").arg(setFileName));
		return;
    }

    bool opened = setFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if(!opened) {
        LOG(QString("Erro while opening %1.").arg(setFileName));
        return;
    }

    QByteArray jsonData = setFile.readAll();
    QJsonParseError error;
    QJsonObject jsonSet = QJsonDocument::fromJson(jsonData, &error).object();
    QString setCode = jsonSet["code"].toString();
    QJsonArray jsonCards = jsonSet["cards"].toArray();

    int aftermathCardsQtd = 0;
    int dualFaceCardsQtd = 0;
    QList<QPair<QString, int>> setCards;
    for(QJsonValueRef jsonCardRef: jsonCards) {
		QJsonObject jsonCard = jsonCardRef.toObject();
        QString number = jsonCard["number"].toString();
        QString layout = jsonCard["layout"].toString();
        // Check for unordered card number
        if(setsUnorderedCardIds[setCode].contains(number)){
            int mtgaID = setsUnorderedCardIds[setCode][number];
            // Create dual aftermath card ex: Dusk /// Dawn
            if(number.endsWith("a") && (layout == "aftermath")){
                QString dualNumber = number.left(number.length()-1);
                setCards.append(QPair<QString, int>(dualNumber, mtgaID-2));
                aftermathCardsQtd += 2;
            }
            setCards.append(QPair<QString, int>(number, mtgaID));
        } else {
            int order = number.toInt() - 1 + aftermathCardsQtd + dualFaceCardsQtd;
            // fix for double-face card parser
            if(layout == "double-faced"){
                QString doubleFaceNumber = number.left(number.length()-1);
                if(number.endsWith("a")){
                    order = doubleFaceNumber.toInt() - 1 + aftermathCardsQtd + dualFaceCardsQtd;
                }
                if(number.endsWith("b")){
                    order = doubleFaceNumber.toInt() + aftermathCardsQtd + dualFaceCardsQtd;
                    dualFaceCardsQtd += 1;
                }
            }
            int mtgaID = setsFirstCardId[setCode] + order * 2;
            setCards.append(QPair<QString, int>(number, mtgaID));
        }
    }

    LOG(QString("%1 set loaded with %2 cards").arg(setCode).arg(jsonCards.count()));

    QString msg = "";
    for(QPair<QString, int> card: setCards){
        msg += QString("{\"%1\", %2}, ").arg(card.first).arg(card.second);
    }
    QString msgIds = msg.left(msg.length()-2);
    LOG(QString("%1: {%2}").arg(setCode).arg(msgIds));
}

QMap<QString, int> MtgaIDs::getAKHUnorderedCardsId()
{
    QMap<QString, int> akhUnorderedCardsId;
    akhUnorderedCardsId["171"] = 65427; //Haze of Pollen was 65141
    akhUnorderedCardsId["172"] = 65141; //Honored Hydra was 65143
    akhUnorderedCardsId["173"] = 65143; //Hooded Brawler was 65145
    akhUnorderedCardsId["174"] = 65145; //Initiate's Companion was 65147
    akhUnorderedCardsId["210a"] = 65221; //Dusk was 65219
    akhUnorderedCardsId["210b"] = 65223; //Dawn was 65221
    akhUnorderedCardsId["211a"] = 65227; //Commit was 65223
    akhUnorderedCardsId["211b"] = 65229; //Memory was 65225
    akhUnorderedCardsId["212a"] = 65233; //Never was 65227
    akhUnorderedCardsId["212b"] = 65235; //Return was 65229
    akhUnorderedCardsId["213a"] = 65239; //Insult was 65231
    akhUnorderedCardsId["213b"] = 65241; //Injury was 65233
    akhUnorderedCardsId["214a"] = 65245; //Mouth was 65235
    akhUnorderedCardsId["214b"] = 65247; //Feed was 65237
    akhUnorderedCardsId["215a"] = 65251; //Start was 65239
    akhUnorderedCardsId["215b"] = 65253; //Finish was 65241
    akhUnorderedCardsId["216a"] = 65257; //Reduce was 65243
    akhUnorderedCardsId["216b"] = 65259; //Rubble was 65245
    akhUnorderedCardsId["217a"] = 65263; //Destined was 65247
    akhUnorderedCardsId["217b"] = 65265; //Lead was 65249
    akhUnorderedCardsId["218a"] = 65269; //Onward was 65251
    akhUnorderedCardsId["218b"] = 65271; //Victory was 65253
    akhUnorderedCardsId["219a"] = 65275; //Spring was 65255
    akhUnorderedCardsId["219b"] = 65277; //Mind was 65257
    akhUnorderedCardsId["220a"] = 65281; //Prepare was 65259
    akhUnorderedCardsId["220b"] = 65283; //Fight was 65261
    akhUnorderedCardsId["221a"] = 65287; //Failure was 65263
    akhUnorderedCardsId["221b"] = 65289; //Comply was 65265
    akhUnorderedCardsId["222a"] = 65293; //Rags was 65267
    akhUnorderedCardsId["222b"] = 65295; //Riches was 65269
    akhUnorderedCardsId["223a"] = 65299; //Cut was 65271
    akhUnorderedCardsId["223b"] = 65301; //Ribbons was 65273
    akhUnorderedCardsId["224a"] = 65305; //Heaven was 65275
    akhUnorderedCardsId["224b"] = 65307; //Earth was 65279
    akhUnorderedCardsId["240"] = 65431; //Cascading Cataracts was 65339
    akhUnorderedCardsId["241"] = 65339; //Cradle of the Accurse was 65341
    akhUnorderedCardsId["242"] = 65341; //Evolving Wilds was 65343
    akhUnorderedCardsId["243"] = 65343; //Fetid Pools was 65345
    akhUnorderedCardsId["244"] = 65345; //Grasping Dunes was 65347
    akhUnorderedCardsId["245"] = 65347; //Irrigated Farmland was 65349
    akhUnorderedCardsId["250"] = 65433; //full-Art Plains was 65359
    akhUnorderedCardsId["251"] = 65435; //full-Art Island was 65361
    akhUnorderedCardsId["252"] = 65437; //full-Art Swamp was 65363
    akhUnorderedCardsId["253"] = 65439; //full-Art Mountain was 65365
    akhUnorderedCardsId["254"] = 65441; //full-Art Forest was 65367
    akhUnorderedCardsId["255"] = 65359; //Plains was 65369
    akhUnorderedCardsId["256"] = 65359; //Plains was 65371
    akhUnorderedCardsId["257"] = 65359; //Plains was 65373
    akhUnorderedCardsId["258"] = 65361; //Island was 65375
    akhUnorderedCardsId["259"] = 65361; //Island was 65377
    akhUnorderedCardsId["260"] = 65361; //Island was 65379
    akhUnorderedCardsId["261"] = 65363; //Swamp was 65381
    akhUnorderedCardsId["262"] = 65363; //Swamp was 65383
    akhUnorderedCardsId["263"] = 65363; //Swamp was 65385
    akhUnorderedCardsId["264"] = 65365; //Mountain was 65387
    akhUnorderedCardsId["265"] = 65365; //Mountain was 65389
    akhUnorderedCardsId["266"] = 65365; //Mountain was 65391
    akhUnorderedCardsId["267"] = 65367; //Forest was 65393
    akhUnorderedCardsId["268"] = 65367; //Forest was 65395
    akhUnorderedCardsId["269"] = 65367; //Forest was 65397
    akhUnorderedCardsId["270"] = 65443; //Gideon, Martial Paragon was 65399
    akhUnorderedCardsId["271"] = 65445; //Companion of the Trials was 65401
    akhUnorderedCardsId["272"] = 65447; //Gideon's Resolve was 65403
    akhUnorderedCardsId["273"] = 65449; //Graceful Cat was 65405
    akhUnorderedCardsId["274"] = 65451; //Stone Quarry was 65407
    akhUnorderedCardsId["275"] = 65453; //Liliana, Death Wielder was 65409
    akhUnorderedCardsId["276"] = 65455; //Desiccated Naga was 65411
    akhUnorderedCardsId["277"] = 65457; //Liliana's Influence was 65413
    akhUnorderedCardsId["278"] = 65459; //Tattered Mummy was 65415
    akhUnorderedCardsId["279"] = 65461; //Foul Orchard was 65417
    akhUnorderedCardsId["280"] = 65463; //Cinder Barrens was 65419
    akhUnorderedCardsId["281"] = 65465; //Forsaken Sanctuary was 65421
    akhUnorderedCardsId["282"] = 65467; //Highland Lake was 65423
    akhUnorderedCardsId["283"] = 65469; //Meandering River was 65425
    akhUnorderedCardsId["284"] = 65471; //Submerged Boneyard was 65427
    akhUnorderedCardsId["285"] = 65473; //Timber Gorge was 65429
    akhUnorderedCardsId["286"] = 65475; //Tranquil Expanse was 65431
    akhUnorderedCardsId["287"] = 65477; //Woodland Stream was 65433
    return akhUnorderedCardsId;
}

QMap<QString, int> MtgaIDs::getHOUUnorderedCardsId()
{
    QMap<QString, int> houUnorderedCardsId;
    houUnorderedCardsId["9"] = 65957; //Disposal Mummy was 65955
    houUnorderedCardsId["10"] = 65495; //Djeru, With Eyes Open was 65497
    houUnorderedCardsId["11"] = 65497; //Djeru's Renunciation was 65499
    houUnorderedCardsId["12"] = 65499; //Dutiful Servants was 65501
    houUnorderedCardsId["58"] = 65595; //Apocalypse Demon was 65593
    houUnorderedCardsId["59"] = 65593; //Banewhip Punisher was 65595
    houUnorderedCardsId["94"] = 65693; //Gilded Cerodon was 65665
    houUnorderedCardsId["95"] = 65691; //Granitic Titan was 65667
    houUnorderedCardsId["96"] = 65667; //Hazoret's Undying Fury was 65669
    houUnorderedCardsId["97"] = 65669; //Hour of Devastation was 65671
    houUnorderedCardsId["98"] = 65671; //Imminent Doom was 65673
    houUnorderedCardsId["99"] = 65673; //Inferno Jet was 65675
    houUnorderedCardsId["100"] = 65675; //Khenra Scrapper was 6577
    houUnorderedCardsId["101"] = 65677; //Kindled Fury was 65679
    houUnorderedCardsId["102"] = 65679; //Magmaroth was 65681
    houUnorderedCardsId["103"] = 65681; //Manticore Eternal was 65683
    houUnorderedCardsId["104"] = 65683; //Neheb, the Eternal was 65685
    houUnorderedCardsId["105"] = 65685; //Open Fire was 65687
    houUnorderedCardsId["106"] = 65687; //Puncturing Blow was 65689
    houUnorderedCardsId["107"] = 65689; //Sand Strangler was 65691
    houUnorderedCardsId["108"] = 65665; //Thorned Moloch was 65693
    houUnorderedCardsId["119"] = 65717; //Hope Tender was 65715
    houUnorderedCardsId["120"] = 65715; //Hour of Promise was 65717
    houUnorderedCardsId["139"] = 65757; //The Locust God was 65755
    houUnorderedCardsId["140"] = 65759; //Nicol Bolas, God-Pharaoh was 65757
    houUnorderedCardsId["141"] = 65761; //Obelisk Spider 65759
    houUnorderedCardsId["142"] = 65763; //Resolute Survivors was 65761
    houUnorderedCardsId["143"] = 65765; //River Hoopoe was 65763
    houUnorderedCardsId["144"] = 65767; //Samut, the Tested was 65765
    houUnorderedCardsId["145"] = 65769; //The Scarab God was 65767
    houUnorderedCardsId["146"] = 65755; //The Scorpion God 65769
    houUnorderedCardsId["148a"] = 65775; //Farm was 65773
    houUnorderedCardsId["148b"] = 65777; //Market was 65775
    houUnorderedCardsId["149a"] = 65781; //Consign was 65777
    houUnorderedCardsId["149b"] = 65783; //Oblivion was 65779
    houUnorderedCardsId["150a"] = 65787; //Claim was 65781
    houUnorderedCardsId["150b"] = 65789; //Fame was 65783
    houUnorderedCardsId["151a"] = 65793; //Struggle was 65785
    houUnorderedCardsId["151b"] = 65795; //Survive was 65787
    houUnorderedCardsId["152a"] = 65799; //Appeal was 65789
    houUnorderedCardsId["152b"] = 65801; //Authority was 65791
    houUnorderedCardsId["153a"] = 65805; //Leave was 65793
    houUnorderedCardsId["153b"] = 65807; //Chance was 65795
    houUnorderedCardsId["154a"] = 65811; //Reason was 65797
    houUnorderedCardsId["154b"] = 65813; //Believe was 65799
    houUnorderedCardsId["155a"] = 65817; //Grind was 65801
    houUnorderedCardsId["155b"] = 65819; //Dust was 65803
    houUnorderedCardsId["156a"] = 65823; //Refuse was 65805
    houUnorderedCardsId["156b"] = 65825; //Cooperate was 65807
    houUnorderedCardsId["157a"] = 65829; //Driven was 65809
    houUnorderedCardsId["157b"] = 65831; //Despair was 65811
    houUnorderedCardsId["169"] = 65959; //Crypt of the Eternals was 65853
    houUnorderedCardsId["185"] = 65887; //Plains was 65887
    houUnorderedCardsId["186"] = 65889; //Island was 65889
    houUnorderedCardsId["187"] = 65891; //Swamp was 65891
    houUnorderedCardsId["188"] = 65893; //Mountain was 65893
    houUnorderedCardsId["189"] = 65895; //Forest was 65895
    houUnorderedCardsId["190"] = 65887; //Plains was 65897
    houUnorderedCardsId["191"] = 65887; //Plains was 65899
    houUnorderedCardsId["192"] = 65889; //Island was 65901
    houUnorderedCardsId["193"] = 65889; //Island was 65903
    houUnorderedCardsId["194"] = 65891; //Swamp was 65905
    houUnorderedCardsId["195"] = 65891; //Swamp was 65907
    houUnorderedCardsId["196"] = 65893; //Mountain was 65909
    houUnorderedCardsId["197"] = 65893; //Mountain was 65911
    houUnorderedCardsId["198"] = 65895; //Forest was 65913
    houUnorderedCardsId["199"] = 65895; //Forest was 65915
    houUnorderedCardsId["200"] = 65937; //Nissa, Genesis Mage was 65917
    houUnorderedCardsId["201"] = 65939; //Avid Reclaimer was 65919
    houUnorderedCardsId["202"] = 65941; //Brambleweft Behemoth was 65921
    houUnorderedCardsId["203"] = 65943; //Nissa's Encouragement was 65923
    houUnorderedCardsId["204"] = 65945; //Woodland Stream was 65925
    houUnorderedCardsId["205"] = 65947; //Nicol Bolas, the Deceiver was 65927
    houUnorderedCardsId["206"] = 65949; //Wasp of the Bitter End was 65929
    houUnorderedCardsId["207"] = 65951; //Zealot of the God-Pharaoh was 65931
    houUnorderedCardsId["208"] = 65953; //Visage of Bolas was 65933
    houUnorderedCardsId["209"] = 65955; //Cinder Barrens was 65935
    return houUnorderedCardsId;
}

QMap<QString, int> MtgaIDs::getXLNUnorderedCardsId()
{
    QMap<QString, int> xlnUnorderedCardsId;
    xlnUnorderedCardsId["260"] = 66499; //Plains was 66499
    xlnUnorderedCardsId["261"] = 66499; //Plains was 66501
    xlnUnorderedCardsId["262"] = 66499; //Plains was 66503
    xlnUnorderedCardsId["263"] = 66499; //Plains was 66505
    xlnUnorderedCardsId["264"] = 66507; //Island was 66507
    xlnUnorderedCardsId["265"] = 66507; //Island was 66509
    xlnUnorderedCardsId["266"] = 66507; //Island was 66511
    xlnUnorderedCardsId["267"] = 66507; //Island was 66513
    xlnUnorderedCardsId["268"] = 66515; //Swamp was 66515
    xlnUnorderedCardsId["269"] = 66515; //Swamp was 66517
    xlnUnorderedCardsId["270"] = 66515; //Swamp was 66519
    xlnUnorderedCardsId["271"] = 66515; //Swamp was 66521
    xlnUnorderedCardsId["272"] = 66523; //Mountain was 66523
    xlnUnorderedCardsId["273"] = 66523; //Mountain was 66525
    xlnUnorderedCardsId["274"] = 66523; //Mountain was 66527
    xlnUnorderedCardsId["275"] = 66523; //Mountain was 66529
    xlnUnorderedCardsId["276"] = 66531; //Forest was 66531
    xlnUnorderedCardsId["277"] = 66531; //Forest was 66533
    xlnUnorderedCardsId["278"] = 66531; //Forest was 66535
    xlnUnorderedCardsId["279"] = 66531; //Forest was 66537
    xlnUnorderedCardsId["280"] = 66541; //Jace, Ingenious Mind-Mage was 66539
    xlnUnorderedCardsId["281"] = 66543; //Castaway's Despair was 66541
    xlnUnorderedCardsId["282"] = 66545; //Grasping Current was 66543
    xlnUnorderedCardsId["283"] = 66547; //Jace's Sentinel was 66545
    xlnUnorderedCardsId["284"] = 66549; //Woodland Stream was 66547
    xlnUnorderedCardsId["285"] = 66551; //Huatli, Dinosaur Knight was 66549
    xlnUnorderedCardsId["286"] = 66553; //Huatli's Snubhorn was 66551
    xlnUnorderedCardsId["287"] = 66555; //Huatli's Spurring was 66553
    xlnUnorderedCardsId["288"] = 66557; //Sun-Blessed Mount was 66555
    xlnUnorderedCardsId["289"] = 66559; //Stone Quarry was 66557
    return xlnUnorderedCardsId;
}

QMap<QString, int> MtgaIDs::getDOMUnorderedCardsId()
{
    QMap<QString, int> domUnorderedCardsId;
    domUnorderedCardsId["250"] = 67604; //Plains was 67604
    domUnorderedCardsId["251"] = 67604; //Plains was 67606
    domUnorderedCardsId["252"] = 67604; //Plains was 67608
    domUnorderedCardsId["253"] = 67604; //Plains was 67610
    domUnorderedCardsId["254"] = 67612; //Island was 67612
    domUnorderedCardsId["255"] = 67612; //Island was 67614
    domUnorderedCardsId["256"] = 67612; //Island was 67616
    domUnorderedCardsId["257"] = 67612; //Island was 67618
    domUnorderedCardsId["258"] = 67620; //Swamp was 67620
    domUnorderedCardsId["259"] = 67620; //Swamp was 67622
    domUnorderedCardsId["260"] = 67620; //Swamp was 67624
    domUnorderedCardsId["261"] = 67620; //Swamp was 67626
    domUnorderedCardsId["262"] = 67628; //Mountain was 67628
    domUnorderedCardsId["263"] = 67628; //Mountain was 67630
    domUnorderedCardsId["264"] = 67628; //Mountain was 67632
    domUnorderedCardsId["265"] = 67628; //Mountain was 67634
    domUnorderedCardsId["266"] = 67636; //Forest was 67636
    domUnorderedCardsId["267"] = 67636; //Forest was 67638
    domUnorderedCardsId["268"] = 67636; //Forest was 67640
    domUnorderedCardsId["269"] = 67636; //Forest was 67642
    domUnorderedCardsId["280"] = 68369; //Firesong and Sunspeaker was 68364
    return domUnorderedCardsId;
}
