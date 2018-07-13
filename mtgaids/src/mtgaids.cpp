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
    loadSet("KLD", 63641, getKLDUnorderedCardsId());
    loadSet("AER", 64213, getAERUnorderedCardsId());
    loadSet("W17", 68414, getW17UnorderedCardsId());
    loadSet("OGW", 62165, {});
    loadSet("AKH", 64801, getAKHUnorderedCardsId());
    loadSet("HOU", 65479, getHOUUnorderedCardsId());
    loadSet("XLN", 65961, getXLNUnorderedCardsId());
    loadSet("RIX", 66619, {});
    loadSet("DOM", 67106, getDOMUnorderedCardsId());
    loadSet("M19", 67682, {});
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

QMap<QString, int> MtgaIDs::getKLDUnorderedCardsId()
{
    QMap<QString, int> kldUnorderedCardsId;
    kldUnorderedCardsId["233"] = 64169; //Thriving Rhino was 64105
    kldUnorderedCardsId["235"] = 64171; //Wild Wanderer was 64109
    kldUnorderedCardsId["251"] = 64139; //Plain was 64141
    kldUnorderedCardsId["252"] = 64139; //Plain was 64143
    kldUnorderedCardsId["254"] = 64145; //Island was 64147
    kldUnorderedCardsId["255"] = 64145; //Island was 64149
    kldUnorderedCardsId["257"] = 64151; //Swamp was 64153
    kldUnorderedCardsId["258"] = 64151; //Swamp was 64155
    kldUnorderedCardsId["260"] = 64157; //Mountain was 64159
    kldUnorderedCardsId["261"] = 64157; //Mountain was 64161
    kldUnorderedCardsId["263"] = 64163; //Forest was 64165
    kldUnorderedCardsId["264"] = 64163; //Forest was 64167
    kldUnorderedCardsId["265"] = 65407; //Chandra, Pyrogenius was 64169
    kldUnorderedCardsId["266"] = 65409; //Flame Lash was 64171
    kldUnorderedCardsId["267"] = 65411; //Liberating Combustion was 64173
    kldUnorderedCardsId["268"] = 65413; //Renegade Firebrand was 64175
    kldUnorderedCardsId["269"] = 65415; //Stone Quarry was 64177
    kldUnorderedCardsId["270"] = 65417; //Nissa, Nature's Artisan was 64179
    kldUnorderedCardsId["271"] = 65419; //Guardian of the Great Conduit was 64181
    kldUnorderedCardsId["272"] = 65421; //Terrain Elemental was 64183
    kldUnorderedCardsId["273"] = 65423; //Verdant Crescendo was 64185
    kldUnorderedCardsId["274"] = 65425; //Woodland Stream was 64187
    return kldUnorderedCardsId;
}

QMap<QString, int> MtgaIDs::getAERUnorderedCardsId()
{
    QMap<QString, int> aerUnorderedCardsId;
    aerUnorderedCardsId["1"] = 64213; //Aerial Modification
    aerUnorderedCardsId["2"] = 64207; //Aeronaut Admiral
    aerUnorderedCardsId["3"] = 64541; //Aether Inspector
    aerUnorderedCardsId["4"] = 64543; //Aethergeode Miner
    aerUnorderedCardsId["5"] = 64211; //Airdrop Aeronauts
    aerUnorderedCardsId["6"] = 64545; //Alley Evasion
    aerUnorderedCardsId["7"] = 64547; //Audacious Infiltrator
    aerUnorderedCardsId["8"] = 64187; //Bastion Enforcer
    aerUnorderedCardsId["9"] = 64219; //Call for Unity
    aerUnorderedCardsId["10"] = 64197; //Caught in the Brights
    aerUnorderedCardsId["11"] = 64549; //Consulate Crackdown
    aerUnorderedCardsId["12"] = 64181; //Conviction
    aerUnorderedCardsId["13"] = 64183; //Countless Gears Renegade
    aerUnorderedCardsId["14"] = 64193; //Dawnfeather Eagle
    aerUnorderedCardsId["15"] = 64203; //Deadeye Harpooner
    aerUnorderedCardsId["16"] = 64189; //Decommission
    aerUnorderedCardsId["17"] = 64209; //Deft Dismissal
    aerUnorderedCardsId["18"] = 64551; //Exquisite Archangel
    aerUnorderedCardsId["19"] = 64205; //Felidar Guardian
    aerUnorderedCardsId["20"] = 64185; //Ghirapur Osprey
    aerUnorderedCardsId["21"] = 64199; //Restoration Specialist
    aerUnorderedCardsId["22"] = 64217; //Solemn Recruit
    aerUnorderedCardsId["23"] = 64215; //Sram, Senior Edificer
    aerUnorderedCardsId["24"] = 64553; //Sram's Expertise
    aerUnorderedCardsId["25"] = 64201; //Thopter Arrest
    aerUnorderedCardsId["26"] = 64555; //Aether Swooper
    aerUnorderedCardsId["27"] = 64271; //Aethertide Whale
    aerUnorderedCardsId["28"] = 64273; //Baral, Chief of Compliance
    aerUnorderedCardsId["29"] = 64557; //Baral's Expertise
    aerUnorderedCardsId["30"] = 64559; //Bastion Inventor
    aerUnorderedCardsId["31"] = 64679; //Disallow
    aerUnorderedCardsId["32"] = 64237; //Dispersal Technician
    aerUnorderedCardsId["33"] = 64561; //Efficient Construction
    aerUnorderedCardsId["34"] = 64233; //Hinterland Drake
    aerUnorderedCardsId["35"] = 64231; //Ice Over
    aerUnorderedCardsId["36"] = 64563; //Illusionist's Stratagem
    aerUnorderedCardsId["37"] = 64245; //Leave in the Dust
    aerUnorderedCardsId["38"] = 64265; //Mechanized Production
    aerUnorderedCardsId["39"] = 64239; //Metallic Rebuke
    aerUnorderedCardsId["40"] = 64235; //Negate
    aerUnorderedCardsId["41"] = 64267; //Quicksmith Spy
    aerUnorderedCardsId["42"] = 64261; //Reverse Engineer
    aerUnorderedCardsId["43"] = 64565; //Salvage Scuttler
    aerUnorderedCardsId["44"] = 64567; //Shielded Aether Thief
    aerUnorderedCardsId["45"] = 64681; //Shipwreck Moray
    aerUnorderedCardsId["46"] = 64249; //Skyship Plunderer
    aerUnorderedCardsId["47"] = 64569; //Take into Custody
    aerUnorderedCardsId["48"] = 64571; //Trophy Mage
    aerUnorderedCardsId["49"] = 64275; //Whir of Invention
    aerUnorderedCardsId["50"] = 64573; //Wind-Kin Raiders
    aerUnorderedCardsId["51"] = 64575; //Aether Poisoner
    aerUnorderedCardsId["52"] = 64281; //Alley Strangler
    aerUnorderedCardsId["53"] = 64319; //Battle at the Bridge
    aerUnorderedCardsId["54"] = 64577; //Cruel Finality
    aerUnorderedCardsId["55"] = 64579; //Daring Demolition
    aerUnorderedCardsId["56"] = 64285; //Defiant Salvager
    aerUnorderedCardsId["57"] = 64311; //Fatal Push
    aerUnorderedCardsId["58"] = 64293; //Fen Hauler
    aerUnorderedCardsId["59"] = 64299; //Foundry Hornet
    aerUnorderedCardsId["60"] = 64297; //Fourth Bridge Prowler
    aerUnorderedCardsId["61"] = 64581; //Gifted Aetherborn
    aerUnorderedCardsId["62"] = 64325; //Glint-Sleeve Siphoner
    aerUnorderedCardsId["63"] = 64583; //Gonti's Machinations
    aerUnorderedCardsId["64"] = 64585; //Herald of Anguish
    aerUnorderedCardsId["65"] = 64317; //Ironclad Revolutionary
    aerUnorderedCardsId["66"] = 64323; //Midnight Entourage
    aerUnorderedCardsId["67"] = 64289; //Night Market Aeronaut
    aerUnorderedCardsId["68"] = 64301; //Perilous Predicament
    aerUnorderedCardsId["69"] = 64587; //Renegade's Getaway
    aerUnorderedCardsId["70"] = 64683; //Resourceful Return
    aerUnorderedCardsId["71"] = 64589; //Secret Salvage
    aerUnorderedCardsId["72"] = 64591; //Sly Requisitioner
    aerUnorderedCardsId["73"] = 64303; //Vengeful Rebel
    aerUnorderedCardsId["74"] = 64315; //Yahenni, Undying Partisan
    aerUnorderedCardsId["75"] = 64593; //Yahenni's Expertise
    aerUnorderedCardsId["76"] = 64595; //Aether Chaser
    aerUnorderedCardsId["77"] = 64597; //Chandra's Revolution
    aerUnorderedCardsId["78"] = 64335; //Destructive Tampering
    aerUnorderedCardsId["79"] = 64601; //Embraal Gear-Smasher
    aerUnorderedCardsId["80"] = 64361; //Enraged Giant
    aerUnorderedCardsId["81"] = 64367; //Freejam Regent
    aerUnorderedCardsId["82"] = 64603; //Frontline Rebel
    aerUnorderedCardsId["83"] = 64357; //Gremlin Infestation
    aerUnorderedCardsId["84"] = 64347; //Hungry Flames
    aerUnorderedCardsId["85"] = 64605; //Indomitable Creativity
    aerUnorderedCardsId["86"] = 64351; //Invigorated Rampage
    aerUnorderedCardsId["87"] = 64607; //Kari Zev, Skyship Raider
    aerUnorderedCardsId["88"] = 64609; //Kari Zev's Expertise
    aerUnorderedCardsId["89"] = 64343; //Lathnu Sailback
    aerUnorderedCardsId["90"] = 64375; //Lightning Runner
    aerUnorderedCardsId["91"] = 64611; //Pia's Revolution
    aerUnorderedCardsId["92"] = 64333; //Precise Strike
    aerUnorderedCardsId["93"] = 64613; //Quicksmith Rebel
    aerUnorderedCardsId["94"] = 64685; //Ravenous Intruder
    aerUnorderedCardsId["95"] = 64615; //Reckless Racer
    aerUnorderedCardsId["96"] = 64373; //Release the Gremlins
    aerUnorderedCardsId["97"] = 64617; //Scrapper Champion
    aerUnorderedCardsId["98"] = 64173; //Shock
    aerUnorderedCardsId["99"] = 64355; //Siege Modification
    aerUnorderedCardsId["100"] = 64339; //Sweatworks Brawler
    aerUnorderedCardsId["101"] = 64345; //Wrangle
    aerUnorderedCardsId["102"] = 64619; //Aether Herder
    aerUnorderedCardsId["103"] = 64621; //Aetherstream Leopard
    aerUnorderedCardsId["104"] = 64687; //Aetherwind Basker
    aerUnorderedCardsId["105"] = 64427; //Aid from the Cowl
    aerUnorderedCardsId["106"] = 64381; //Druid of the Cowl
    aerUnorderedCardsId["107"] = 64623; //Greenbelt Rampager
    aerUnorderedCardsId["108"] = 64421; //Greenwheel Liberator
    aerUnorderedCardsId["109"] = 64415; //Heroic Intervention
    aerUnorderedCardsId["110"] = 64625; //Hidden Herbalists
    aerUnorderedCardsId["111"] = 64379; //Highspire Infusion
    aerUnorderedCardsId["112"] = 64413; //Lifecraft Awakening
    aerUnorderedCardsId["113"] = 64391; //Lifecraft Cavalry
    aerUnorderedCardsId["114"] = 64627; //Lifecrafter's Gift
    aerUnorderedCardsId["115"] = 64399; //Maulfist Revolutionary
    aerUnorderedCardsId["116"] = 64405; //Monstrous Onslaught
    aerUnorderedCardsId["117"] = 64629; //Narnam Renegade
    aerUnorderedCardsId["118"] = 64383; //Natural Obsolescence
    aerUnorderedCardsId["119"] = 64401; //Peema Aether-Seer
    aerUnorderedCardsId["120"] = 64631; //Prey Upon
    aerUnorderedCardsId["121"] = 64633; //Ridgescale Tusker
    aerUnorderedCardsId["122"] = 64417; //Rishkar, Peema Renegade
    aerUnorderedCardsId["123"] = 64635; //Rishkar's Expertise
    aerUnorderedCardsId["124"] = 64637; //Scrounging Bandar
    aerUnorderedCardsId["125"] = 64403; //Silkweaver Elite
    aerUnorderedCardsId["126"] = 64387; //Unbridled Growth
    aerUnorderedCardsId["127"] = 64455; //Ajani Unyielding
    aerUnorderedCardsId["128"] = 64639; //Dark Intimations
    aerUnorderedCardsId["129"] = 64443; //Hidden Stockpile
    aerUnorderedCardsId["130"] = 64641; //Maverick Thopterist
    aerUnorderedCardsId["131"] = 64449; //Oath of Ajani
    aerUnorderedCardsId["132"] = 64437; //Outland Boar
    aerUnorderedCardsId["133"] = 64435; //Renegade Rallier
    aerUnorderedCardsId["134"] = 64643; //Renegade Wheelsmith
    aerUnorderedCardsId["135"] = 64645; //Rogue Refiner
    aerUnorderedCardsId["136"] = 64445; //Spire Patrol
    aerUnorderedCardsId["137"] = 64647; //Tezzeret the Schemer
    aerUnorderedCardsId["138"] = 64649; //Tezzeret's Touch
    aerUnorderedCardsId["139"] = 64431; //Weldfast Engineer
    aerUnorderedCardsId["140"] = 64651; //Winding Constrictor
    aerUnorderedCardsId["141"] = 64467; //Aegis Automaton
    aerUnorderedCardsId["142"] = 64689; //Aethersphere Harvester
    aerUnorderedCardsId["143"] = 64481; //Augmenting Automaton
    aerUnorderedCardsId["144"] = 64515; //Barricade Breaker
    aerUnorderedCardsId["145"] = 64509; //Cogwork Assembler
    aerUnorderedCardsId["146"] = 64501; //Consulate Dreadnought
    aerUnorderedCardsId["147"] = 64507; //Consulate Turret
    aerUnorderedCardsId["148"] = 64655; //Crackdown Construct
    aerUnorderedCardsId["149"] = 64523; //Daredevil Dragster
    aerUnorderedCardsId["150"] = 64657; //Filigree Crawler
    aerUnorderedCardsId["151"] = 64495; //Foundry Assembler
    aerUnorderedCardsId["152"] = 64659; //Gonti's Aether Heart
    aerUnorderedCardsId["153"] = 64535; //Heart of Kiran
    aerUnorderedCardsId["154"] = 64661; //Hope of Ghirapur
    aerUnorderedCardsId["155"] = 64461; //Implement of Combustion
    aerUnorderedCardsId["156"] = 64479; //Implement of Examination
    aerUnorderedCardsId["157"] = 64469; //Implement of Ferocity
    aerUnorderedCardsId["158"] = 64691; //Implement of Improvement
    aerUnorderedCardsId["159"] = 64475; //Implement of Malice
    aerUnorderedCardsId["160"] = 64663; //Inspiring Statuary
    aerUnorderedCardsId["161"] = 64485; //Irontread Crusher
    aerUnorderedCardsId["162"] = 64665; //Lifecrafter's Bestiary
    aerUnorderedCardsId["163"] = 64667; //Merchant's Dockhand
    aerUnorderedCardsId["164"] = 64669; //Metallic Mimic
    aerUnorderedCardsId["165"] = 64473; //Mobile Garrison
    aerUnorderedCardsId["166"] = 64477; //Night Market Guard
    aerUnorderedCardsId["167"] = 64497; //Ornithopter
    aerUnorderedCardsId["168"] = 64503; //Pacification Array
    aerUnorderedCardsId["169"] = 64533; //Paradox Engine
    aerUnorderedCardsId["170"] = 64527; //Peacewalker Colossus
    aerUnorderedCardsId["171"] = 64539; //Planar Bridge
    aerUnorderedCardsId["172"] = 64671; //Prizefighter Construct
    aerUnorderedCardsId["173"] = 64459; //Renegade Map
    aerUnorderedCardsId["174"] = 64491; //Reservoir Walker
    aerUnorderedCardsId["175"] = 64521; //Scrap Trawler
    aerUnorderedCardsId["176"] = 64673; //Servo Schematic
    aerUnorderedCardsId["177"] = 64513; //Treasure Keeper
    aerUnorderedCardsId["178"] = 64463; //Universal Solvent
    aerUnorderedCardsId["179"] = 64505; //Untethered Express
    aerUnorderedCardsId["180"] = 64487; //Verdant Automaton
    aerUnorderedCardsId["181"] = 64675; //Walking Ballista
    aerUnorderedCardsId["182"] = 64483; //Watchful Automaton
    aerUnorderedCardsId["183"] = 64677; //Welder Automaton
    aerUnorderedCardsId["184"] = 64531; //Spire of Industry
    aerUnorderedCardsId["185"] = 65917; //Ajani, Valiant Protector
    aerUnorderedCardsId["186"] = 65919; //Inspiring Roar
    aerUnorderedCardsId["187"] = 65921; //Ajani's Comrade
    aerUnorderedCardsId["188"] = 65923; //Ajani's Aid
    aerUnorderedCardsId["189"] = 65925; //Tranquil Expanse
    aerUnorderedCardsId["190"] = 65927; //Tezzeret, Master of Metal
    aerUnorderedCardsId["191"] = 65929; //Tezzeret's Betrayal
    aerUnorderedCardsId["192"] = 65931; //Pendulum of Patterns
    aerUnorderedCardsId["193"] = 65933; //Tezzeret's Simulacrum
    aerUnorderedCardsId["194"] = 65935; //Submerged Boneyard
    return aerUnorderedCardsId;
}

QMap<QString, int> MtgaIDs::getW17UnorderedCardsId()
{
    QMap<QString, int> w17UnorderedCardsId;
    w17UnorderedCardsId["2"] = 68415; //Glory Seeker was 68416
    w17UnorderedCardsId["3"] = 68416; //Serra Angel was 68418
    w17UnorderedCardsId["4"] = 68417; //Standing Troops was 68420
    w17UnorderedCardsId["5"] = 68418; //Stormfront Pegasus was 68422
    w17UnorderedCardsId["6"] = 68419; //Victory's Herald was 68424
    w17UnorderedCardsId["7"] = 68420; //Air Elemental was 68426
    w17UnorderedCardsId["8"] = 68421; //Coral Merfolk was 68428
    w17UnorderedCardsId["9"] = 68422; //Drag Under was 68430
    w17UnorderedCardsId["10"] = 68423; //Inspiration was 68432
    w17UnorderedCardsId["11"] = 68424; //Sleep Paralysis was 68434
    w17UnorderedCardsId["12"] = 68425; //Sphinx of Magosi was 68436
    w17UnorderedCardsId["13"] = 68426; //Stealer of Secrets was 68438
    w17UnorderedCardsId["14"] = 68427; //Tricks of the Trade was 68440
    w17UnorderedCardsId["15"] = 68428; //Bloodhunter Bat was 68442
    w17UnorderedCardsId["16"] = 68429; //Certain Death was 68444
    w17UnorderedCardsId["17"] = 68430; //Nightmare was 68446
    w17UnorderedCardsId["18"] = 68431; //Raise Dead was 68448
    w17UnorderedCardsId["19"] = 68432; //Sengir Vampire was 68450
    w17UnorderedCardsId["20"] = 68433; //Untamed Hunger was 68452
    w17UnorderedCardsId["21"] = 68434; //Falkenrath Reaver was 68454
    w17UnorderedCardsId["22"] = 68435; //Shivan Dragon was 68456
    w17UnorderedCardsId["23"] = 68436; //Thundering Giant was 68458
    w17UnorderedCardsId["24"] = 68437; //Garruk's Horde was 68460
    w17UnorderedCardsId["25"] = 68438; //Oakenform was 68462
    w17UnorderedCardsId["26"] = 68439; //Rabid Bite was 68464
    w17UnorderedCardsId["27"] = 68440; //Rootwalla was 68466
    w17UnorderedCardsId["28"] = 68441; //Stalking Tiger was 68468
    w17UnorderedCardsId["29"] = 68442; //Stampeding Rhino was 68470
    w17UnorderedCardsId["30"] = 68443; //Wing Snare was 68472
    return w17UnorderedCardsId;
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
