#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <cassert>
#include <random>
#include <tuple>

namespace ecs
{

	using id = size_t;
	using entitites = std::vector<id>;

	using name = std::string;
	using isMale = bool;
	using detail = std::pair<name, isMale>;
	using detail_component = std::pair<id, detail>;
	using details = std::vector<detail_component>;

	enum class type { fish, seaweed };
	using hp = int8_t;
	using age = int8_t;
	using type_component = std::tuple<id, type, hp, age>;
	using types = std::vector<type_component>;

	enum class race { merou, thon, clown, sole, bar, carpe };
	using race_component = std::pair<id, race>;
	using races = std::vector<race_component>;

	entitites _entities;
	details _details;
	types _types;
	races _races;

	std::random_device _random_device;
	std::default_random_engine _random_engine{ _random_device() };

	bool is_fish(type const& t) { return (t == type::fish); }
	bool is_seaweed(type const& t) { return(t == type::seaweed); }
	bool is_herbivorous(race const& r) { return(r == race::merou || r == race::thon || r == race::clown); }
	bool is_carnivorous(race const& r) { return(r == race::sole || r == race::bar || r == race::carpe); }
	bool is_hermaphrodite(race const& r) { return(r == race::bar || r == race::merou); }
	bool is_oportunistic(race const& r) { return (r == race::sole || r == race::clown); }

	bool is_fish_t(type_component const& t) { return is_fish(std::get<1>(t)); }
	bool is_seaweed_t(type_component const& t) { return is_seaweed(std::get<1>(t)); }
	bool is_herbivorous_p(race_component const& r) { return is_herbivorous(r.second); }
	bool is_carnivorous_p(race_component const& r) { return is_carnivorous(r.second); }
	bool is_hermaphrodite_p(race_component const& r) { return is_hermaphrodite(r.second); }
	bool is_opportunistic_p(race_component const& r) { return is_oportunistic(r.second); }

	hp & get_hp(type_component & t) { return std::get<2>(t); }
	const hp get_const_hp(type_component & t) { return std::get<2>(t); }
	age & get_age(type_component & t) { return std::get<3>(t); }
	const age get_const_age(type_component & t) { return std::get<3>(t); }
	bool is_famished(type_component & t) { return ((get_const_hp(t) <= 5) ? true : false); }
	bool is_dead(type_component & t) { return ((get_const_hp(t) <= 0) ? true : false); }
	bool is_hold(type_component & t) { return ((get_const_age(t) >= 20) ? true : false); }
	bool is_half_life(type_component & t) { return (get_const_age(t) == 10); }
	bool are_both_race(race_component const& r1, race_component const& r2) { return (r1.second == r2.second); }
	bool are_both_sexe(detail_component const& r1, detail_component const& r2) { return (r1.second.second == r2.second.second); }

	std::string type_string_t(type_component t)
	{
		if (std::get<1>(t) == type::fish)
		{
			return "Fish";
		}

		return "SeaWeed";
	}

	std::string to_string(race r)
	{
		const std::string races[]{ "Mérou", "Thon", "Clown", "Sole", "Bar", "Carpe" };
		return races[static_cast<size_t>(r)];
	}

	bool random_sexe()
	{
		std::uniform_int_distribution<size_t> distribution(0, 1);

		return(distribution(_random_engine) == 0);
	}

	race random_race()
	{
		std::uniform_int_distribution<size_t> distribution(0, 5);
		return static_cast<race>(distribution(_random_engine));
	}

	id random_entity(std::uniform_int_distribution<size_t> & dist, std::vector<id> const& entities)
	{
		return entities[dist(ecs::_random_engine)];
	}

	void swap_sexe(detail_component & d)
	{
		d.second.second == true ? d.second.second = false : d.second.second = true;
	}

	id create_entity()
	{
		const auto id = _entities.empty() ? 1 : _entities.back() + 1;
		_entities.push_back(id);

		return id;
	}

	id add_seaweed(int hp = 10)
	{
		const auto id{ create_entity() };
		_types.push_back(std::make_tuple(id, type::seaweed, hp, 0));

		return id;
	}

	id add_fish(name & name, race r, isMale sexe)
	{
		const auto id{ create_entity() };

		_types.push_back(std::make_tuple(id, type::fish, 10, 0));
		_details.push_back(std::make_pair(id, std::make_pair(name, sexe)));
		_races.push_back(std::make_pair(id, r));

		return id;
	}

	template<typename Collection>
	typename Collection::value_type & get_component(Collection & collection, id id)
	{
		auto it = std::find_if(begin(collection), end(collection),
			[id](auto p) {return(std::get<0>(p) == id); });

		return (*it);
	}

	template<typename Function>
	std::vector<id> get_entities(Function&& function)
	{
		std::vector<id> entities;
		std::for_each(_types.begin(), _types.end(),
			[&entities, function](auto p) { if (function(std::get<1>(p))) { entities.push_back(std::get<0>(p)); } });

		return entities;
	}

	const std::vector<id> get_fishs()
	{
		return(get_entities(ecs::is_fish));
	}

	const std::vector<id> get_seaweeds()
	{
		return(get_entities(ecs::is_seaweed));
	}

	const std::vector<id> get_herbivorous(std::vector<id> const& fishs)
	{
		std::vector<id> herbivorous;

		for (auto & fish : fishs)
		{
			auto fish_race{ get_component(_races, fish) };
			if (is_herbivorous_p(fish_race))
			{
				herbivorous.push_back(fish);
			}
		}

		return herbivorous;
	}

	const std::vector<id> get_carnivorous(std::vector<id> const& fishs)
	{
		std::vector<id> carnivorous;

		for (auto & fish : fishs)
		{
			auto fish_race{ get_component(_races, fish) };
			if (is_carnivorous_p(fish_race))
			{
				carnivorous.push_back(fish);
			}
		}

		return carnivorous;
	}

	void delete_entity(id id)
	{
		const auto entities_it = std::remove(_entities.begin(), _entities.end(), id);
		_entities.erase(entities_it, _entities.end());

		const auto details_it = std::remove_if(_details.begin(), _details.end(),
			[id](auto p) { return (p.first == id); });
		_details.erase(details_it, _details.end());

		const auto types_it = std::remove_if(_types.begin(), _types.end(),
			[id](auto p) { return (std::get<0>(p) == id); });
		_types.erase(types_it, _types.end());

		const auto races_it = std::remove_if(_races.begin(), _races.end(),
			[id](auto p) { return (p.first == id); });
		_races.erase(races_it, _races.end());
	}

	void delete_entities(std::vector<id> & entities)
	{
		for (auto & entity : entities)
		{
			delete_entity(entity);
		}
	}

	void hp_update()
	{
		std::vector<id> entity_to_delete;

		for (auto const& entity : ecs::_entities)
		{
			auto & type{ get_component(ecs::_types, entity) };
			if (ecs::is_fish_t(type))
			{
				ecs::get_hp(type) -= 1;
			}
			else
			{
				ecs::get_hp(type) += 1;
			}

			if (is_dead(type)) { entity_to_delete.push_back(entity); }
		}

		delete_entities(entity_to_delete);
	}

	void age_update()
	{
		std::vector<id> entity_to_delete;

		for (auto const& entity : _entities)
		{
			auto & type{ get_component(ecs::_types, entity) };

			get_age(type) += 1;

			if (is_hold(type)) { entity_to_delete.push_back(entity); }
		}

		delete_entities(entity_to_delete);
	}

	void sexe_update(id entity)
	{
		const auto race{ get_component(ecs::_races, entity) };
		auto type{ get_component(ecs::_types, entity) };
		if (is_half_life(type) && is_hermaphrodite_p(race))
		{
			auto & sexe{ get_component(_details, entity) };
			swap_sexe(sexe);
		}
	}

	void eat_fish(type_component & eater_type, id target, std::vector<id> & trash)
	{
		auto & target_type{ ecs::get_component(ecs::_types, target) };

		if (!ecs::is_dead(target_type))
		{
			ecs::get_hp(eater_type) += 5;
			ecs::get_hp(target_type) -= 4;

			if (ecs::is_dead(target_type))
			{
				trash.push_back(target);
			}
		}
	}

	void eat_seaweed(type_component & eater_type, id target, std::vector<id> & trash)
	{
		auto & target_type{ ecs::get_component(ecs::_types, target) };

		if (!is_dead(target_type))
		{
			ecs::get_hp(eater_type) += 3;
			ecs::get_hp(target_type) -= 2;

			if (is_dead(target_type))
			{
				trash.push_back(target);
			}
		}
	}

	void fish_reproduce(id fish, id target)
	{
		auto target_type{ ecs::get_component(ecs::_types, target) };
		const auto target_race{ ecs::get_component(ecs::_races, target) };
		const auto fish_race{ ecs::get_component(ecs::_races, fish) };

		if (!ecs::is_dead(target_type) && ecs::are_both_race(target_race, fish_race))
		{
			auto & fish_detail{ ecs::get_component(ecs::_details, fish) };
			auto & target_detail{ ecs::get_component(ecs::_details, target) };

			if (!ecs::are_both_sexe(fish_detail, target_detail))
			{
				ecs::add_fish(ecs::to_string(fish_race.second), fish_race.second, ecs::random_sexe());
			}
			else if (ecs::is_opportunistic_p(target_race))
			{
				ecs::swap_sexe(target_detail);
				ecs::add_fish(ecs::to_string(fish_race.second), fish_race.second, ecs::random_sexe());
			}

		}
	}

	void seaweed_reproduce(std::vector<id> const& seaweeds)
	{
		for (auto & seaweed : seaweeds)
		{
			auto & seaweed_type{ ecs::get_component(ecs::_types, seaweed) };
			auto & seaweed_hp{ ecs::get_hp(seaweed_type) };
			if (!ecs::is_dead(seaweed_type) && seaweed_hp >= 10)
			{
				seaweed_hp /= 2;
				ecs::add_seaweed(seaweed_hp);
			}
		}
	}

	void print(size_t tour, size_t entities, size_t seaweed, size_t herbivorous, size_t carnivorous)
	{
		std::cout << tour << "\t" << entities << "\t\t" << seaweed << "\t\t" << herbivorous << "\t\t" << carnivorous << std::endl;
	}

	void spend_time(std::vector<id> const& fishs, std::vector<id> const& seaweeds)
	{
		ecs::hp_update();
		ecs::age_update();

		std::uniform_int_distribution<size_t> fish_dist(0, fishs.size() - 1);
		std::uniform_int_distribution<size_t> seaweed_dist(0, seaweeds.size() - 1);

		std::vector<ecs::id> entity_dead;

		for (auto & fish : fishs)
		{
			sexe_update(fish);
			auto & fish_type{ ecs::get_component(ecs::_types, fish) };

			if (ecs::is_famished(fish_type) && !ecs::is_dead(fish_type))
			{
				auto fish_race{ ecs::get_component(ecs::_races, fish) };

				if (ecs::is_carnivorous(fish_race.second) && !fishs.empty())
				{
					ecs::eat_fish(fish_type, ecs::random_entity(fish_dist, fishs), entity_dead);
				}
				else if (ecs::is_herbivorous(fish_race.second) && !seaweeds.empty())
				{
					ecs::eat_seaweed(fish_type, ecs::random_entity(seaweed_dist, seaweeds), entity_dead);
				}
			}
			else if (!ecs::is_famished(fish_type) && !ecs::is_dead(fish_type))
			{
				fish_reproduce(fish, random_entity(fish_dist, fishs));
			}

		}

		ecs::seaweed_reproduce(seaweeds);

		ecs::delete_entities(entity_dead);
	}
}

int main() {

	for (size_t i{ 0 }; i < 5; i++) { ecs::add_seaweed(); }
	for (size_t i{ 0 }; i < 10; i++)
	{
		const auto race{ ecs::random_race() };
		ecs::add_fish(ecs::to_string(race), race, ecs::random_sexe());
	}

	std::cout << "Tourt\tEntitees\tAlgue\tHerbivores\tCarnivores" << std::endl;

	for (size_t tour{}; tour < 20; ++tour)
	{
		const auto fishs{ ecs::get_fishs() };
		const auto seaweeds{ ecs::get_seaweeds() };

		ecs::spend_time(fishs, seaweeds);

		const auto herbivorous{ ecs::get_herbivorous(fishs) };
		const auto carnivorous{ ecs::get_carnivorous(fishs) };
		ecs::print(tour, ecs::_entities.size(), seaweeds.size(), herbivorous.size(), carnivorous.size());
	}

	std::cout << "===== Time Out ===== " << std::endl;

	return 0;
}