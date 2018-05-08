#include "mtgcards.h"
#include "../transformations.h"
#include "../macros.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonValueRef>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>

MtgCards::MtgCards(QObject *parent) : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if(RUNNING_TESTS){
        dataDir = ":res";
    }
    setsDir = dataDir + QDir::separator() + "sets";
    if (!QFile::exists(setsDir)) {
        QDir dir;
        dir.mkpath(setsDir);
    }

    mtgaIds["OGW"] = {{"1", 62165}, {"2", 62167}, {"3", 62169}, {"4", 62171}, {"5", 62173}, {"6", 62175}, {"7", 62177}, {"8", 62179}, {"9", 62181}, {"10", 62183}, {"11", 62185}, {"12", 62187}, {"13", 62189}, {"14", 62191}, {"15", 62193}, {"16", 62195}, {"17", 62197}, {"18", 62199}, {"19", 62201}, {"20", 62203}, {"21", 62205}, {"22", 62207}, {"23", 62209}, {"24", 62211}, {"25", 62213}, {"26", 62215}, {"27", 62217}, {"28", 62219}, {"29", 62221}, {"30", 62223}, {"31", 62225}, {"32", 62227}, {"33", 62229}, {"34", 62231}, {"35", 62233}, {"36", 62235}, {"37", 62237}, {"38", 62239}, {"39", 62241}, {"40", 62243}, {"41", 62245}, {"42", 62247}, {"43", 62249}, {"44", 62251}, {"45", 62253}, {"46", 62255}, {"47", 62257}, {"48", 62259}, {"49", 62261}, {"50", 62263}, {"51", 62265}, {"52", 62267}, {"53", 62269}, {"54", 62271}, {"55", 62273}, {"56", 62275}, {"57", 62277}, {"58", 62279}, {"59", 62281}, {"60", 62283}, {"61", 62285}, {"62", 62287}, {"63", 62289}, {"64", 62291}, {"65", 62293}, {"66", 62295}, {"67", 62297}, {"68", 62299}, {"69", 62301}, {"70", 62303}, {"71", 62305}, {"72", 62307}, {"73", 62309}, {"74", 62311}, {"75", 62313}, {"76", 62315}, {"77", 62317}, {"78", 62319}, {"79", 62321}, {"80", 62323}, {"81", 62325}, {"82", 62327}, {"83", 62329}, {"84", 62331}, {"85", 62333}, {"86", 62335}, {"87", 62337}, {"88", 62339}, {"89", 62341}, {"90", 62343}, {"91", 62345}, {"92", 62347}, {"93", 62349}, {"94", 62351}, {"95", 62353}, {"96", 62355}, {"97", 62357}, {"98", 62359}, {"99", 62361}, {"100", 62363}, {"101", 62365}, {"102", 62367}, {"103", 62369}, {"104", 62371}, {"105", 62373}, {"106", 62375}, {"107", 62377}, {"108", 62379}, {"109", 62381}, {"110", 62383}, {"111", 62385}, {"112", 62387}, {"113", 62389}, {"114", 62391}, {"115", 62393}, {"116", 62395}, {"117", 62397}, {"118", 62399}, {"119", 62401}, {"120", 62403}, {"121", 62405}, {"122", 62407}, {"123", 62409}, {"124", 62411}, {"125", 62413}, {"126", 62415}, {"127", 62417}, {"128", 62419}, {"129", 62421}, {"130", 62423}, {"131", 62425}, {"132", 62427}, {"133", 62429}, {"134", 62431}, {"135", 62433}, {"136", 62435}, {"137", 62437}, {"138", 62439}, {"139", 62441}, {"140", 62443}, {"141", 62445}, {"142", 62447}, {"143", 62449}, {"144", 62451}, {"145", 62453}, {"146", 62455}, {"147", 62457}, {"148", 62459}, {"149", 62461}, {"150", 62463}, {"151", 62465}, {"152", 62467}, {"153", 62469}, {"154", 62471}, {"155", 62473}, {"156", 62475}, {"157", 62477}, {"158", 62479}, {"159", 62481}, {"160", 62483}, {"161", 62485}, {"162", 62487}, {"163", 62489}, {"164", 62491}, {"165", 62493}, {"166", 62495}, {"167", 62497}, {"168", 62499}, {"169", 62501}, {"170", 62503}, {"171", 62505}, {"172", 62507}, {"173", 62509}, {"174", 62511}, {"175", 62513}, {"176", 62515}, {"177", 62517}, {"178", 62519}, {"179", 62521}, {"180", 62523}, {"181", 62525}, {"182", 62527}, {"183", 62529}, {"183", 62529}, {"184", 62531}, {"184", 62531}};
    mtgaIds["AKH"] = {{"1", 64801}, {"2", 64803}, {"3", 64805}, {"4", 64807}, {"5", 64809}, {"6", 64811}, {"7", 64813}, {"8", 64815}, {"9", 64817}, {"10", 64819}, {"11", 64821}, {"12", 64823}, {"13", 64825}, {"14", 64827}, {"15", 64829}, {"16", 64831}, {"17", 64833}, {"18", 64835}, {"19", 64837}, {"20", 64839}, {"21", 64841}, {"22", 64843}, {"23", 64845}, {"24", 64847}, {"25", 64849}, {"26", 64851}, {"27", 64853}, {"28", 64855}, {"29", 64857}, {"30", 64859}, {"31", 64861}, {"32", 64863}, {"33", 64865}, {"34", 64867}, {"35", 64869}, {"36", 64871}, {"37", 64873}, {"38", 64875}, {"39", 64877}, {"40", 64879}, {"41", 64881}, {"42", 64883}, {"43", 64885}, {"44", 64887}, {"45", 64889}, {"46", 64891}, {"47", 64893}, {"48", 64895}, {"49", 64897}, {"50", 64899}, {"51", 64901}, {"52", 64903}, {"53", 64905}, {"54", 64907}, {"55", 64909}, {"56", 64911}, {"57", 64913}, {"58", 64915}, {"59", 64917}, {"60", 64919}, {"61", 64921}, {"62", 64923}, {"63", 64925}, {"64", 64927}, {"65", 64929}, {"66", 64931}, {"67", 64933}, {"68", 64935}, {"69", 64937}, {"70", 64939}, {"71", 64941}, {"72", 64943}, {"73", 64945}, {"74", 64947}, {"75", 64949}, {"76", 64951}, {"77", 64953}, {"78", 64955}, {"79", 64957}, {"80", 64959}, {"81", 64961}, {"82", 64963}, {"83", 64965}, {"84", 64967}, {"85", 64969}, {"86", 64971}, {"87", 64973}, {"88", 64975}, {"89", 64977}, {"90", 64979}, {"91", 64981}, {"92", 64983}, {"93", 64985}, {"94", 64987}, {"95", 64989}, {"96", 64991}, {"97", 64993}, {"98", 64995}, {"99", 64997}, {"100", 64999}, {"101", 65001}, {"102", 65003}, {"103", 65005}, {"104", 65007}, {"105", 65009}, {"106", 65011}, {"107", 65013}, {"108", 65015}, {"109", 65017}, {"110", 65019}, {"111", 65021}, {"112", 65023}, {"113", 65025}, {"114", 65027}, {"115", 65029}, {"116", 65031}, {"117", 65033}, {"118", 65035}, {"119", 65037}, {"120", 65039}, {"121", 65041}, {"122", 65043}, {"123", 65045}, {"124", 65047}, {"125", 65049}, {"126", 65051}, {"127", 65053}, {"128", 65055}, {"129", 65057}, {"130", 65059}, {"131", 65061}, {"132", 65063}, {"133", 65065}, {"134", 65067}, {"135", 65069}, {"136", 65071}, {"137", 65073}, {"138", 65075}, {"139", 65077}, {"140", 65079}, {"141", 65081}, {"142", 65083}, {"143", 65085}, {"144", 65087}, {"145", 65089}, {"146", 65091}, {"147", 65093}, {"148", 65095}, {"149", 65097}, {"150", 65099}, {"151", 65101}, {"152", 65103}, {"153", 65105}, {"154", 65107}, {"155", 65109}, {"156", 65111}, {"157", 65113}, {"158", 65115}, {"159", 65117}, {"160", 65119}, {"161", 65121}, {"162", 65123}, {"163", 65125}, {"164", 65127}, {"165", 65129}, {"166", 65131}, {"167", 65133}, {"168", 65135}, {"169", 65137}, {"170", 65139}, {"171", 65427}, {"172", 65141}, {"173", 65143}, {"174", 65145}, {"175", 65149}, {"176", 65151}, {"177", 65153}, {"178", 65155}, {"179", 65157}, {"180", 65159}, {"181", 65161}, {"182", 65163}, {"183", 65165}, {"184", 65167}, {"185", 65169}, {"186", 65171}, {"187", 65173}, {"188", 65175}, {"189", 65177}, {"190", 65179}, {"191", 65181}, {"192", 65183}, {"193", 65185}, {"194", 65187}, {"195", 65189}, {"196", 65191}, {"197", 65193}, {"198", 65195}, {"199", 65197}, {"200", 65199}, {"201", 65201}, {"202", 65203}, {"203", 65205}, {"204", 65207}, {"205", 65209}, {"206", 65211}, {"207", 65213}, {"208", 65215}, {"209", 65217}, {"210", 65219}, {"210a", 65221}, {"210b", 65223}, {"211", 65225}, {"211a", 65227}, {"211b", 65229}, {"212", 65231}, {"212a", 65233}, {"212b", 65235}, {"213", 65237}, {"213a", 65239}, {"213b", 65241}, {"214", 65243}, {"214a", 65245}, {"214b", 65247}, {"215", 65249}, {"215a", 65251}, {"215b", 65253}, {"216", 65255}, {"216a", 65257}, {"216b", 65259}, {"217", 65261}, {"217a", 65263}, {"217b", 65265}, {"218", 65267}, {"218a", 65269}, {"218b", 65271}, {"219", 65273}, {"219a", 65275}, {"219b", 65277}, {"220", 65279}, {"220a", 65281}, {"220b", 65283}, {"221", 65285}, {"221a", 65287}, {"221b", 65289}, {"222", 65291}, {"222a", 65293}, {"222b", 65295}, {"223", 65297}, {"223a", 65299}, {"223b", 65301}, {"224", 65303}, {"224a", 65305}, {"224b", 65307}, {"225", 65309}, {"226", 65311}, {"227", 65313}, {"228", 65315}, {"229", 65317}, {"230", 65319}, {"231", 65321}, {"232", 65323}, {"233", 65325}, {"234", 65327}, {"235", 65329}, {"236", 65331}, {"237", 65333}, {"238", 65335}, {"239", 65337}, {"240", 65431}, {"241", 65339}, {"242", 65341}, {"243", 65343}, {"244", 65345}, {"245", 65347}, {"246", 65351}, {"247", 65353}, {"248", 65355}, {"249", 65357}, {"250", 65433}, {"251", 65435}, {"252", 65437}, {"253", 65439}, {"254", 65441}, {"255", 65359}, {"256", 65359}, {"257", 65359}, {"258", 65361}, {"259", 65361}, {"260", 65361}, {"261", 65363}, {"262", 65363}, {"263", 65363}, {"264", 65365}, {"265", 65365}, {"266", 65365}, {"267", 65367}, {"268", 65367}, {"269", 65367}, {"270", 65443}, {"271", 65445}, {"272", 65447}, {"273", 65449}, {"274", 65451}, {"275", 65453}, {"276", 65455}, {"277", 65457}, {"278", 65459}, {"279", 65461}, {"280", 65463}, {"281", 65465}, {"282", 65467}, {"283", 65469}, {"284", 65471}, {"285", 65473}, {"286", 65475}, {"287", 65477}};
    mtgaIds["HOU"] = {{"1", 65479}, {"2", 65481}, {"3", 65483}, {"4", 65485}, {"5", 65487}, {"6", 65489}, {"7", 65491}, {"8", 65493}, {"9", 65957}, {"10", 65495}, {"11", 65497}, {"12", 65499}, {"13", 65503}, {"14", 65505}, {"15", 65507}, {"16", 65509}, {"17", 65511}, {"18", 65513}, {"19", 65515}, {"20", 65517}, {"21", 65519}, {"22", 65521}, {"23", 65523}, {"24", 65525}, {"25", 65527}, {"26", 65529}, {"27", 65531}, {"28", 65533}, {"29", 65535}, {"30", 65537}, {"31", 65539}, {"32", 65541}, {"33", 65543}, {"34", 65545}, {"35", 65547}, {"36", 65549}, {"37", 65551}, {"38", 65553}, {"39", 65555}, {"40", 65557}, {"41", 65559}, {"42", 65561}, {"43", 65563}, {"44", 65565}, {"45", 65567}, {"46", 65569}, {"47", 65571}, {"48", 65573}, {"49", 65575}, {"50", 65577}, {"51", 65579}, {"52", 65581}, {"53", 65583}, {"54", 65585}, {"55", 65587}, {"56", 65589}, {"57", 65591}, {"58", 65595}, {"59", 65593}, {"60", 65597}, {"61", 65599}, {"62", 65601}, {"63", 65603}, {"64", 65605}, {"65", 65607}, {"66", 65609}, {"67", 65611}, {"68", 65613}, {"69", 65615}, {"70", 65617}, {"71", 65619}, {"72", 65621}, {"73", 65623}, {"74", 65625}, {"75", 65627}, {"76", 65629}, {"77", 65631}, {"78", 65633}, {"79", 65635}, {"80", 65637}, {"81", 65639}, {"82", 65641}, {"83", 65643}, {"84", 65645}, {"85", 65647}, {"86", 65649}, {"87", 65651}, {"88", 65653}, {"89", 65655}, {"90", 65657}, {"91", 65659}, {"92", 65661}, {"93", 65663}, {"94", 65693}, {"95", 65691}, {"96", 65667}, {"97", 65669}, {"98", 65671}, {"99", 65673}, {"100", 65675}, {"101", 65677}, {"102", 65679}, {"103", 65681}, {"104", 65683}, {"105", 65685}, {"106", 65687}, {"107", 65689}, {"108", 65665}, {"109", 65695}, {"110", 65697}, {"111", 65699}, {"112", 65701}, {"113", 65703}, {"114", 65705}, {"115", 65707}, {"116", 65709}, {"117", 65711}, {"118", 65713}, {"119", 65717}, {"120", 65715}, {"121", 65719}, {"122", 65721}, {"123", 65723}, {"124", 65725}, {"125", 65727}, {"126", 65729}, {"127", 65731}, {"128", 65733}, {"129", 65735}, {"130", 65737}, {"131", 65739}, {"132", 65741}, {"133", 65743}, {"134", 65745}, {"135", 65747}, {"136", 65749}, {"137", 65751}, {"138", 65753}, {"139", 65757}, {"140", 65759}, {"141", 65761}, {"142", 65763}, {"143", 65765}, {"144", 65767}, {"145", 65769}, {"146", 65755}, {"147", 65771}, {"148", 65773}, {"148a", 65775}, {"148b", 65777}, {"149", 65779}, {"149a", 65781}, {"149b", 65783}, {"150", 65785}, {"150a", 65787}, {"150b", 65789}, {"151", 65791}, {"151a", 65793}, {"151b", 65795}, {"152", 65797}, {"152a", 65799}, {"152b", 65801}, {"153", 65803}, {"153a", 65805}, {"153b", 65807}, {"154", 65809}, {"154a", 65811}, {"154b", 65813}, {"155", 65815}, {"155a", 65817}, {"155b", 65819}, {"156", 65821}, {"156a", 65823}, {"156b", 65825}, {"157", 65827}, {"157a", 65829}, {"157b", 65831}, {"158", 65833}, {"159", 65835}, {"160", 65837}, {"161", 65839}, {"162", 65841}, {"163", 65843}, {"164", 65845}, {"165", 65847}, {"166", 65849}, {"167", 65851}, {"168", 65853}, {"169", 65959}, {"170", 65857}, {"171", 65859}, {"172", 65861}, {"173", 65863}, {"174", 65865}, {"175", 65867}, {"176", 65869}, {"177", 65871}, {"178", 65873}, {"179", 65875}, {"180", 65877}, {"181", 65879}, {"182", 65881}, {"183", 65883}, {"184", 65885}, {"185", 65887}, {"186", 65889}, {"187", 65891}, {"188", 65893}, {"189", 65895}, {"190", 65887}, {"191", 65887}, {"192", 65889}, {"193", 65889}, {"194", 65891}, {"195", 65891}, {"196", 65893}, {"197", 65893}, {"198", 65895}, {"199", 65895}, {"200", 65937}, {"201", 65939}, {"202", 65941}, {"203", 65943}, {"204", 65945}, {"205", 65947}, {"206", 65949}, {"207", 65951}, {"208", 65953}, {"209", 65955}};
    mtgaIds["XLN"] = {{"1", 65961}, {"2", 65963}, {"3", 65965}, {"4", 65967}, {"5", 65969}, {"6", 65971}, {"7", 65973}, {"8", 65975}, {"9", 65977}, {"10", 65979}, {"11", 65981}, {"12", 65983}, {"13", 65985}, {"14", 65987}, {"15", 65989}, {"16", 65991}, {"17", 65993}, {"18", 65995}, {"19", 65997}, {"20", 65999}, {"21", 66001}, {"22a", 66003}, {"22b", 66005}, {"23", 66007}, {"24", 66009}, {"25", 66011}, {"26", 66013}, {"27", 66015}, {"28", 66017}, {"29", 66019}, {"30", 66021}, {"31", 66023}, {"32", 66025}, {"33", 66027}, {"34", 66029}, {"35", 66031}, {"36", 66033}, {"37", 66035}, {"38", 66037}, {"39", 66039}, {"40", 66041}, {"41", 66043}, {"42", 66045}, {"43", 66047}, {"44", 66049}, {"45", 66051}, {"46", 66053}, {"47", 66055}, {"48", 66057}, {"49", 66059}, {"50", 66061}, {"51", 66063}, {"52", 66065}, {"53", 66067}, {"54", 66069}, {"55", 66071}, {"56", 66073}, {"57", 66075}, {"58", 66077}, {"59", 66079}, {"60", 66081}, {"61", 66083}, {"62", 66085}, {"63", 66087}, {"64", 66089}, {"65", 66091}, {"66", 66093}, {"67", 66095}, {"68", 66097}, {"69", 66099}, {"70", 66101}, {"71", 66103}, {"72", 66105}, {"73", 66107}, {"74a", 66109}, {"74b", 66111}, {"75", 66113}, {"76", 66115}, {"77", 66117}, {"78", 66119}, {"79", 66121}, {"80", 66123}, {"81", 66125}, {"82", 66127}, {"83", 66129}, {"84", 66131}, {"85", 66133}, {"86", 66135}, {"87", 66137}, {"88", 66139}, {"89", 66141}, {"90a", 66143}, {"90b", 66145}, {"91", 66147}, {"92", 66149}, {"93", 66151}, {"94", 66153}, {"95", 66155}, {"96", 66157}, {"97", 66159}, {"98", 66161}, {"99", 66163}, {"100", 66165}, {"101", 66167}, {"102", 66169}, {"103", 66171}, {"104", 66173}, {"105", 66175}, {"106", 66177}, {"107", 66179}, {"108", 66181}, {"109", 66183}, {"110", 66185}, {"111", 66187}, {"112", 66189}, {"113", 66191}, {"114", 66193}, {"115", 66195}, {"116", 66197}, {"117", 66199}, {"118", 66201}, {"119", 66203}, {"120", 66205}, {"121", 66207}, {"122", 66209}, {"123", 66211}, {"124", 66213}, {"125", 66215}, {"126", 66217}, {"127", 66219}, {"128", 66221}, {"129", 66223}, {"130", 66225}, {"131", 66227}, {"132", 66229}, {"133", 66231}, {"134", 66233}, {"135", 66235}, {"136", 66237}, {"137", 66239}, {"138", 66241}, {"139", 66243}, {"140", 66245}, {"141", 66247}, {"142", 66249}, {"143", 66251}, {"144", 66253}, {"145", 66255}, {"146", 66257}, {"147", 66259}, {"148", 66261}, {"149", 66263}, {"150", 66265}, {"151", 66267}, {"152", 66269}, {"153", 66271}, {"154", 66273}, {"155", 66275}, {"156", 66277}, {"157", 66279}, {"158", 66281}, {"159", 66283}, {"160", 66285}, {"161", 66287}, {"162", 66289}, {"163", 66291}, {"164", 66293}, {"165", 66295}, {"166", 66297}, {"167", 66299}, {"168", 66301}, {"169", 66303}, {"170", 66305}, {"171", 66307}, {"172", 66309}, {"173a", 66311}, {"173b", 66313}, {"174", 66315}, {"175", 66317}, {"176", 66319}, {"177", 66321}, {"178", 66323}, {"179", 66325}, {"180", 66327}, {"181", 66329}, {"182", 66331}, {"183", 66333}, {"184", 66335}, {"185", 66337}, {"186", 66339}, {"187", 66341}, {"188", 66343}, {"189", 66345}, {"190", 66347}, {"191a", 66349}, {"191b", 66351}, {"192", 66353}, {"193", 66355}, {"194", 66357}, {"195", 66359}, {"196", 66361}, {"197", 66363}, {"198", 66365}, {"199", 66367}, {"200", 66369}, {"201", 66371}, {"202", 66373}, {"203", 66375}, {"204", 66377}, {"205", 66379}, {"206", 66381}, {"207", 66383}, {"208", 66385}, {"209", 66387}, {"210", 66389}, {"211", 66391}, {"212", 66393}, {"213", 66395}, {"214", 66397}, {"215", 66399}, {"216", 66401}, {"217", 66403}, {"218", 66405}, {"219", 66407}, {"220", 66409}, {"221", 66411}, {"222", 66413}, {"223", 66415}, {"224", 66417}, {"225", 66419}, {"226", 66421}, {"227", 66423}, {"228", 66425}, {"229", 66427}, {"230", 66429}, {"231", 66431}, {"232", 66433}, {"233", 66435}, {"234a", 66437}, {"234b", 66439}, {"235a", 66441}, {"235b", 66443}, {"236", 66445}, {"237", 66447}, {"238", 66449}, {"239", 66451}, {"240", 66453}, {"241", 66455}, {"242", 66457}, {"243a", 66459}, {"243b", 66461}, {"244", 66463}, {"245", 66465}, {"246", 66467}, {"247", 66469}, {"248", 66471}, {"249a", 66473}, {"249b", 66475}, {"250a", 66477}, {"250b", 66479}, {"251", 66481}, {"252", 66483}, {"253", 66485}, {"254", 66487}, {"255", 66489}, {"256", 66491}, {"257", 66493}, {"258", 66495}, {"259", 66497}, {"260", 66499}, {"261", 66499}, {"262", 66499}, {"263", 66499}, {"264", 66507}, {"265", 66507}, {"266", 66507}, {"267", 66507}, {"268", 66515}, {"269", 66515}, {"270", 66515}, {"271", 66515}, {"272", 66523}, {"273", 66523}, {"274", 66523}, {"275", 66523}, {"276", 66531}, {"277", 66531}, {"278", 66531}, {"279", 66531}, {"280", 66541}, {"281", 66543}, {"282", 66545}, {"283", 66547}, {"284", 66549}, {"285", 66551}, {"286", 66553}, {"287", 66555}, {"288", 66557}, {"289", 66559}};
    mtgaIds["RIX"] = {{"1", 66619}, {"2", 66621}, {"3", 66623}, {"4", 66625}, {"5", 66627}, {"6", 66629}, {"7", 66631}, {"8", 66633}, {"9", 66635}, {"10", 66637}, {"11", 66639}, {"12", 66641}, {"13", 66643}, {"14", 66645}, {"15", 66647}, {"16", 66649}, {"17", 66651}, {"18", 66653}, {"19", 66655}, {"20", 66657}, {"21", 66659}, {"22", 66661}, {"23", 66663}, {"24", 66665}, {"25", 66667}, {"26", 66669}, {"27", 66671}, {"28", 66673}, {"29", 66675}, {"30", 66677}, {"31", 66679}, {"32", 66681}, {"33", 66683}, {"34", 66685}, {"35", 66687}, {"36", 66689}, {"37", 66691}, {"38", 66693}, {"39", 66695}, {"40", 66697}, {"41", 66699}, {"42", 66701}, {"43", 66703}, {"44", 66705}, {"45", 66707}, {"46", 66709}, {"47", 66711}, {"48", 66713}, {"49", 66715}, {"50", 66717}, {"51", 66719}, {"52", 66721}, {"53", 66723}, {"54", 66725}, {"55", 66727}, {"56", 66729}, {"57", 66731}, {"58", 66733}, {"59", 66735}, {"60", 66737}, {"61", 66739}, {"62", 66741}, {"63", 66743}, {"64", 66745}, {"65", 66747}, {"66", 66749}, {"67", 66751}, {"68", 66753}, {"69", 66755}, {"70", 66757}, {"71", 66759}, {"72", 66761}, {"73", 66763}, {"74", 66765}, {"75", 66767}, {"76", 66769}, {"77", 66771}, {"78", 66773}, {"79", 66775}, {"80", 66777}, {"81", 66779}, {"82", 66781}, {"83", 66783}, {"84", 66785}, {"85", 66787}, {"86", 66789}, {"87", 66791}, {"88", 66793}, {"89", 66795}, {"90", 66797}, {"91", 66799}, {"92", 66801}, {"93", 66803}, {"94", 66805}, {"95", 66807}, {"96", 66809}, {"97", 66811}, {"98", 66813}, {"99", 66815}, {"100", 66817}, {"101", 66819}, {"102", 66821}, {"103", 66823}, {"104", 66825}, {"105", 66827}, {"106", 66829}, {"107", 66831}, {"108", 66833}, {"109", 66835}, {"110", 66837}, {"111", 66839}, {"112", 66841}, {"113", 66843}, {"114", 66845}, {"115", 66847}, {"116", 66849}, {"117", 66851}, {"118", 66853}, {"119", 66855}, {"120", 66857}, {"121", 66859}, {"122", 66861}, {"123", 66863}, {"124", 66865}, {"125", 66867}, {"126", 66869}, {"127", 66871}, {"128", 66873}, {"129", 66875}, {"130", 66877}, {"131", 66879}, {"132", 66881}, {"133", 66883}, {"134", 66885}, {"135", 66887}, {"136", 66889}, {"137", 66891}, {"138", 66893}, {"139", 66895}, {"140", 66897}, {"141", 66899}, {"142", 66901}, {"143", 66903}, {"144", 66905}, {"145", 66907}, {"146", 66909}, {"147", 66911}, {"148", 66913}, {"149", 66915}, {"150", 66917}, {"151", 66919}, {"152", 66921}, {"153", 66923}, {"154", 66925}, {"155", 66927}, {"156", 66929}, {"157", 66931}, {"158a", 66933}, {"158b", 66935}, {"159", 66937}, {"160a", 66939}, {"160b", 66941}, {"161", 66943}, {"162", 66945}, {"163", 66947}, {"164", 66949}, {"165a", 66951}, {"165b", 66953}, {"166a", 66955}, {"166b", 66957}, {"167", 66959}, {"168", 66961}, {"169", 66963}, {"170", 66965}, {"171", 66967}, {"172", 66969}, {"173a", 66971}, {"173b", 66973}, {"174", 66975}, {"175", 66977}, {"176a", 66979}, {"176b", 66981}, {"177", 66983}, {"178", 66985}, {"179a", 66987}, {"179b", 66989}, {"180", 66991}, {"181", 66993}, {"182", 66995}, {"183", 66997}, {"184", 66999}, {"185", 67001}, {"186", 67003}, {"187", 67005}, {"188", 67007}, {"189", 67009}, {"190", 67011}, {"191", 67013}, {"192", 67015}, {"193", 67017}, {"194", 67019}, {"195", 67021}, {"196", 67023}, {"197", 67025}, {"198", 67027}, {"199", 67029}, {"200", 67031}, {"201", 67033}, {"202", 67035}, {"203", 67037}, {"204", 67039}, {"205", 67041}};
    mtgaIds["DOM"] = {{"1", 67106}, {"2", 67108}, {"3", 67110}, {"4", 67112}, {"5", 67114}, {"6", 67116}, {"7", 67118}, {"8", 67120}, {"9", 67122}, {"10", 67124}, {"11", 67126}, {"12", 67128}, {"13", 67130}, {"14", 67132}, {"15", 67134}, {"16", 67136}, {"17", 67138}, {"18", 67140}, {"19", 67142}, {"20", 67144}, {"21", 67146}, {"22", 67148}, {"23", 67150}, {"24", 67152}, {"25", 67154}, {"26", 67156}, {"27", 67158}, {"28", 67160}, {"29", 67162}, {"30", 67164}, {"31", 67166}, {"32", 67168}, {"33", 67170}, {"34", 67172}, {"35", 67174}, {"36", 67176}, {"37", 67178}, {"38", 67180}, {"39", 67182}, {"40", 67184}, {"41", 67186}, {"42", 67188}, {"43", 67190}, {"44", 67192}, {"45", 67194}, {"46", 67196}, {"47", 67198}, {"48", 67200}, {"49", 67202}, {"50", 67204}, {"51", 67206}, {"52", 67208}, {"53", 67210}, {"54", 67212}, {"55", 67214}, {"56", 67216}, {"57", 67218}, {"58", 67220}, {"59", 67222}, {"60", 67224}, {"61", 67226}, {"62", 67228}, {"63", 67230}, {"64", 67232}, {"65", 67234}, {"66", 67236}, {"67", 67238}, {"68", 67240}, {"69", 67242}, {"70", 67244}, {"71", 67246}, {"72", 67248}, {"73", 67250}, {"74", 67252}, {"75", 67254}, {"76", 67256}, {"77", 67258}, {"78", 67260}, {"79", 67262}, {"80", 67264}, {"81", 67266}, {"82", 67268}, {"83", 67270}, {"84", 67272}, {"85", 67274}, {"86", 67276}, {"87", 67278}, {"88", 67280}, {"89", 67282}, {"90", 67284}, {"91", 67286}, {"92", 67288}, {"93", 67290}, {"94", 67292}, {"95", 67294}, {"96", 67296}, {"97", 67298}, {"98", 67300}, {"99", 67302}, {"100", 67304}, {"101", 67306}, {"102", 67308}, {"103", 67310}, {"104", 67312}, {"105", 67314}, {"106", 67316}, {"107", 67318}, {"108", 67320}, {"109", 67322}, {"110", 67324}, {"111", 67326}, {"112", 67328}, {"113", 67330}, {"114", 67332}, {"115", 67334}, {"116", 67336}, {"117", 67338}, {"118", 67340}, {"119", 67342}, {"120", 67344}, {"121", 67346}, {"122", 67348}, {"123", 67350}, {"124", 67352}, {"125", 67354}, {"126", 67356}, {"127", 67358}, {"128", 67360}, {"129", 67362}, {"130", 67364}, {"131", 67366}, {"132", 67368}, {"133", 67370}, {"134", 67372}, {"135", 67374}, {"136", 67376}, {"137", 67378}, {"138", 67380}, {"139", 67382}, {"140", 67384}, {"141", 67386}, {"142", 67388}, {"143", 67390}, {"144", 67392}, {"145", 67394}, {"146", 67396}, {"147", 67398}, {"148", 67400}, {"149", 67402}, {"150", 67404}, {"151", 67406}, {"152", 67408}, {"153", 67410}, {"154", 67412}, {"155", 67414}, {"156", 67416}, {"157", 67418}, {"158", 67420}, {"159", 67422}, {"160", 67424}, {"161", 67426}, {"162", 67428}, {"163", 67430}, {"164", 67432}, {"165", 67434}, {"166", 67436}, {"167", 67438}, {"168", 67440}, {"169", 67442}, {"170", 67444}, {"171", 67446}, {"172", 67448}, {"173", 67450}, {"174", 67452}, {"175", 67454}, {"176", 67456}, {"177", 67458}, {"178", 67460}, {"179", 67462}, {"180", 67464}, {"181", 67466}, {"182", 67468}, {"183", 67470}, {"184", 67472}, {"185", 67474}, {"186", 67476}, {"187", 67478}, {"188", 67480}, {"189", 67482}, {"190", 67484}, {"191", 67486}, {"192", 67488}, {"193", 67490}, {"194", 67492}, {"195", 67494}, {"196", 67496}, {"197", 67498}, {"198", 67500}, {"199", 67502}, {"200", 67504}, {"201", 67506}, {"202", 67508}, {"203", 67510}, {"204", 67512}, {"205", 67514}, {"206", 67516}, {"207", 67518}, {"208", 67520}, {"209", 67522}, {"210", 67524}, {"211", 67526}, {"212", 67528}, {"213", 67530}, {"214", 67532}, {"215", 67534}, {"216", 67536}, {"217", 67538}, {"218", 67540}, {"219", 67542}, {"220", 67544}, {"221", 67546}, {"222", 67548}, {"223", 67550}, {"224", 67552}, {"225", 67554}, {"226", 67556}, {"227", 67558}, {"228", 67560}, {"229", 67562}, {"230", 67564}, {"231", 67566}, {"232", 67568}, {"233", 67570}, {"234", 67572}, {"235", 67574}, {"236", 67576}, {"237", 67578}, {"238", 67580}, {"239", 67582}, {"240", 67584}, {"241", 67586}, {"242", 67588}, {"243", 67590}, {"244", 67592}, {"245", 67594}, {"246", 67596}, {"247", 67598}, {"248", 67600}, {"249", 67602}, {"250", 67604}, {"251", 67604}, {"252", 67604}, {"253", 67604}, {"254", 67612}, {"255", 67612}, {"256", 67612}, {"257", 67612}, {"258", 67620}, {"259", 67620}, {"260", 67620}, {"261", 67620}, {"262", 67628}, {"263", 67628}, {"264", 67628}, {"265", 67628}, {"266", 67636}, {"267", 67636}, {"268", 67636}, {"269", 67636}, {"270", 67644}, {"271", 67646}, {"272", 67648}, {"273", 67650}, {"274", 67652}, {"275", 67654}, {"276", 67656}, {"277", 67658}, {"278", 67660}, {"279", 67662}, {"280", 68369}};
	
    for (QString setCode : mtgaIds.keys()){
        loadSet(setCode);	
    }
}

Card* MtgCards::findCard(int mtgaId)
{
    return cards[mtgaId];
}

void MtgCards::loadSet(QString setCode)
{
    QFile setFile(setsDir + QDir::separator() + setCode + ".json");
    if (QFileInfo(setFile).exists()) {
          loadSetFromFile(setCode + ".json");
    } else {
          downloadSet(setCode);
    }
}

void MtgCards::downloadSet(QString setCode)
{
    QString setUrl = QString("https://mtgjson.com/json/%1.json").arg(setCode);
    QNetworkRequest request(setUrl);
    QNetworkReply *reply = networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &MtgCards::downloadSetOnFinish);
    LOGD(QString("Downloading %1 cards from %2").arg(setCode).arg(setUrl));
}

void MtgCards::downloadSetOnFinish()
{
  	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
  	QByteArray jsonData = reply->readAll();

    if (reply->error() == QNetworkReply::ContentNotFoundError) {
        QString setUrl = reply->url().toString();
        LOGW(QString("Error while downloading mtg card json: %1").arg(setUrl));
        return;
  	}

    QJsonObject jsonSet = Transformations::stringToJsonObject(jsonData);
    if (jsonSet.empty()) {
        return;
    }
    QString setCode = jsonSet["code"].toString();
    LOGD(QString("Downloaded %1 bytes from %2 json").arg(jsonData.size()).arg(setCode));

    QFile setFile(setsDir + QDir::separator() + setCode + ".json");
    setFile.open(QIODevice::WriteOnly);
    setFile.write(jsonData);
    setFile.close();

    loadSetFromFile(setCode + ".json");
}

void MtgCards::loadSetFromFile(QString setFileName) {
    LOGD(QString("Loading %1").arg(setFileName));

    QFile setFile(setsDir + QDir::separator() + setFileName);
    if (!QFileInfo(setFile).exists()) {
        LOGW(QString("%1 not found.").arg(setFileName));
		return;
    }

    bool opened = setFile.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!opened) {
        LOGW(QString("Erro while opening %1.").arg(setFileName));
        return;
    }

    QByteArray jsonData = setFile.readAll();
    QJsonObject jsonSet = Transformations::stringToJsonObject(jsonData);
    if (jsonSet.empty()) {
        return;
    }
    QString setCode = jsonSet["code"].toString();
    QJsonArray jsonCards = jsonSet["cards"].toArray();

    for (QJsonValueRef jsonCardRef: jsonCards) {
		QJsonObject jsonCard = jsonCardRef.toObject();
        Card* card = jsonObject2Card(jsonCard, setCode);
        cards[card->mtgaId] = card;
    }

    LOGI(QString("%1 set loaded with %2 cards").arg(setCode).arg(jsonCards.count()));
}

Card* MtgCards::jsonObject2Card(QJsonObject jsonCard, QString setCode)
{
    QString number = jsonCard["number"].toString();
    QString name = jsonCard["name"].toString();
    QString type = jsonCard["type"].toString();
    QJsonArray jsonTypes = jsonCard["types"].toArray();
    bool isLand = false;
    for (QJsonValueRef typeRef : jsonTypes) {
        if (typeRef.toString() == "Land") {
            isLand = true;
            break;
        }
    }
    int mtgaId = mtgaIds[setCode][number];
    // Mana color
    QString rawManaCost = jsonCard["manaCost"].toString();
    QRegularExpression reManaSymbol("(?<=\\{)\\w(?=\\})");
    QRegularExpressionMatchIterator iterator = reManaSymbol.globalMatch(rawManaCost);
    QString manaCost;
    while (iterator.hasNext()) {
        manaCost += iterator.next().captured(0).toLower().at(0);
    }
    // Mana color identity
    QString text = jsonCard["text"].toString();
    QList<QChar> manaColorIdentity;
    if (isLand) {
        QJsonArray jsonColorIdentity = jsonCard["colorIdentity"].toArray();
        for (QJsonValueRef colorIdentityRef : jsonColorIdentity) {
            manaColorIdentity << colorIdentityRef.toString().toLower().at(0);
        }
        if (manaColorIdentity.isEmpty()) {
            manaColorIdentity << QChar(text.contains("mana of any color") ? 'm' : 'c');
        }
    } else {
        manaColorIdentity = manaCost2ManaColorIdentity(manaCost);
    }
    return new Card(mtgaId, setCode, number, name, type, manaCost, manaColorIdentity, isLand);
}

QList<QChar> MtgCards::manaCost2ManaColorIdentity(QString manaCost)
{
    QList<QChar> manaSymbols;
    for (QChar manaSymbol : manaCost) {
        if (manaSymbol.isLetter()){
            if (!manaSymbols.contains(manaSymbol)){
                manaSymbols << manaSymbol;
            }
        }
    }
    if (manaSymbols.isEmpty()) {
        manaSymbols << 'a';
    } else if (manaSymbols.size() >= 4) {
        manaSymbols.clear();
        manaSymbols << QChar('m');
    }
    return manaSymbols;
}
