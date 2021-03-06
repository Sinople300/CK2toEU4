#include "EU4Country.hh"

#include "CK2Ruler.hh"
#include "CK2Title.hh"
#include "EU4Province.hh"

const string EU4Country::kNoProvinceMarker("has_zero_provs");

EU4Country::EU4Country (Object* o)
  : Enumerable<EU4Country>(this, o->getKey(), false)
  , ObjectWrapper(o)
  , ckSovereign(0)
  , ckTitle(0)
{}

void EU4Country::addProvince(EU4Province* prov) {
  if (find(provinces.begin(), provinces.end(), prov) == provinces.end()) {
    Logger::logStream("countries")
        << "Adding " << prov->getName() << " to " << getKey() << "\n";
    provinces.push_back(prov);
  }
}

bool EU4Country::converts () {
  if (!getRuler()) return false;
  if (safeGetString(kNoProvinceMarker, PlainNone) == "yes") return false;
  return true;
}

bool EU4Country::isROTW() const {
  return getRuler() == nullptr;
}

std::string EU4Country::getGovernment() {
  return getNeededObject("government")
      ->safeGetString("government", "\"feudal_monarchy\"");
}

void EU4Country::setGovernment(const std::string& government) {
  getNeededObject("government")->resetLeaf("government", government);
  resetHistory("government", remQuotes(government));
}

void EU4Country::setAsCore (EU4Province* prov) {
  Object* coreList = getNeededObject("core_provinces");
  bool listed = false;
  string provTag = prov->getKey();
  for (int i = 0; i < coreList->numTokens(); ++i) {
    if (coreList->getToken(i) != provTag) continue;
    listed = true;
    break;
  }
  if (!listed) coreList->addToList(provTag);

  if (!prov->hasCore(getKey())) {
    prov->addCore(getKey());
  }
}

void EU4Country::removeCore (EU4Province* prov) {
  getNeededObject("core_provinces")->remToken(prov->getKey());
  prov->remCore(getKey());
}

void EU4Country::remProvince(EU4Province* prov) {
  if (find(provinces.begin(), provinces.end(), prov) != provinces.end()) {
    Logger::logStream("countries")
        << "Removing " << prov->getName() << " from " << getKey() << "\n";
    REMOVE(provinces, prov);
  }
}

void EU4Country::setRuler (CK2Ruler* ruler, CK2Title* title) {
  ckSovereign = ruler;
  if (title == ruler->getPrimaryTitle()) ruler->setEU4Country(this);
  ckTitle = title;
  title->setEU4Country(this);
}
