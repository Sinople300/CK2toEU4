#include "CK2War.hh"

#include <unordered_map>

#include "Logger.hh"

string createUniqueWarName(Object* obj) {
  static std::unordered_map<string, int> nameCount;
  string warName = obj->safeGetString("name");
  nameCount[warName]++;
  if (nameCount[warName] > 1) {
    sprintf(strbuffer, "%s %i", warName.c_str(), nameCount[warName]);
    warName = strbuffer;
  }
  return warName;
}

CK2War::CK2War(Object* obj)
    : Enumerable<CK2War>(this, createUniqueWarName(obj), false),
      ObjectWrapper(obj) {
  objvec attack = object->getValue("attacker");
  for (objiter attacker = attack.begin(); attacker != attack.end(); ++attacker) {
    CK2Ruler* ruler = CK2Ruler::findByName((*attacker)->getLeaf());
    if (!ruler) {
      Logger::logStream(LogStream::Warn) << "Could not find character "
					 << (*attacker)->getLeaf()
					 << ", attacker in war "
					 << getName()
					 << ".\n";
      continue;
    }
    attackers.push_back(ruler);
  }
  objvec defend = object->getValue("defender");
  for (objiter defender = defend.begin(); defender != defend.end(); ++defender) {
    CK2Ruler* ruler = CK2Ruler::findByName((*defender)->getLeaf());
    if (!ruler) {
      Logger::logStream(LogStream::Warn) << "Could not find character "
					 << (*defender)->getLeaf()
					 << ", defender in war "
					 << getName()
					 << ".\n";
      continue;
    }
    defenders.push_back(ruler);
  }

  for (auto* attacker : attackers) {
    for (auto* defender : defenders) {
      Logger::logStream(LogStream::Info)
          << attacker->getName() << " is now enemy of " << defender->getName()
          << "\n";
      attacker->addEnemy(defender);
      defender->addEnemy(attacker);
    }
  }
}
