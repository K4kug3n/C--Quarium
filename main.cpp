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

	struct detail
	{
		name name_data;
		isMale sexe_data;
	};
	struct detail_component
	{
		detail detail_data;
		id id_data;
	};
	using details = std::vector<detail_component>;

	enum class type { fish, seaweed };
	using hp = int8_t;
	using age = int8_t;

	struct living
	{
		type type_data;
		hp hp_data;
		age age_data;
	};
	struct living_component
	{
		living living_data;
		id id_data;
	};
	using types = std::vector<living_component>;

	enum class race { merou, thon, clown, sole, bar, carpe };
	struct race_component
	{
		race race_data;
		id id_data;
	};
	using races = std::vector<race_component>;

	struct Aquarium
	{
		entitites _entities;
		details _details;
		types _types;
		races _races;
	};

	std::random_device _random_device;
	std::default_random_engine _random_engine{ _random_device() };


	bool is_fish(type const& t) { return (t == type::fish); }
	bool is_seaweed(type const& t) { return(t == type::seaweed); }
	bool is_herbivorous(race const& r) { return(r == race::merou || r == race::thon || r == race::clown); }
	bool is_carnivorous(race const& r) { return(r == race::sole || r == race::bar || r == race::carpe); }
	bool is_hermaphrodite(race const& r) { return(r == race::bar || r == race::merou); }
	bool is_oportunistic(race const& r) { return (r == race::sole || r == race::clown); }

	bool is_fish_t(living_component const& t) { return is_fish(t.living_data.type_data); }
	bool is_seaweed_t(living_component const& t) { return is_seaweed(t.living_data.type_data); }
	bool is_herbivorous_p(race_component const& r) { return is_herbivorous(r.race_data); }
	bool is_carnivorous_p(race_component const& r) { return is_carnivorous(r.race_data); }
	bool is_hermaphrodite_p(race_component const& r) { return is_hermaphrodite(r.race_data); }
	bool is_opportunistic_p(race_component const& r) { return is_oportunistic(r.race_data); }

	hp & get_hp(living_component & t) { return t.living_data.hp_data; }
	const hp get_const_hp(living_component & t) { return t.living_data.hp_data; }
	age & get_age(living_component & t) { return t.living_data.age_data; }
	const age get_const_age(living_component & t) { return t.living_data.age_data; }
	bool is_famished(living_component & t) { return ((get_const_hp(t) <= 5) ? true : false); }
	bool is_dead(living_component & t) { return ((get_const_hp(t) <= 0) ? true : false); }
	bool is_hold(living_component & t) { return ((get_const_age(t) >= 20) ? true : false); }
	bool is_half_life(living_component & t) { return (get_const_age(t) == 10); }
	bool are_both_race(race_component const& r1, race_component const& r2) { return (r1.race_data == r2.race_data); }
	bool are_both_sexe(detail_component const& r1, detail_component const& r2) { return (r1.detail_data.sexe_data == r2.detail_data.sexe_data); }

	std::string type_string_t(living_component t)
	{
		if (t.living_data.type_data == type::fish)
		{
			return "Fish";
		}

		return "SeaWeed";
	}

	std::string to_string(race r)
	{
		const std::string races[]{ "M�rou", "Thon", "Clown", "Sole", "Bar", "Carpe" };
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
		return entities[dist(_random_engine)];
	}

	void swap_sexe(detail_component & d)
	{
		d.detail_data.sexe_data == true ? d.detail_data.sexe_data = false : d.detail_data.sexe_data = true;
	}

	id create_entity(Aquarium & aquarium)
	{
		const auto id = aquarium._entities.empty() ? 1 : aquarium._entities.back() + 1;
		aquarium._entities.push_back(id);

		return id;
	}

	id add_seaweed(Aquarium & aquarium, hp hp = 10)
	{
		const auto id{ create_entity(aquarium) };
		aquarium._types.push_back(living_component{ living{ type::seaweed, hp, 0 }, id });

		return id;
	}

	id add_fish(Aquarium & aquarium, name & name, race r, isMale sexe)
	{
		const auto id{ create_entity(aquarium) };

		aquarium._types.push_back(living_component{ living{ type::fish, 10, 0 }, id });
		aquarium._details.push_back(detail_component{ detail{ name, sexe }, id });
		aquarium._races.push_back(race_component{ r, id });

		return id;
	}

	template<typename Collection>
	typename Collection::value_type & get_component(Collection & collection, id id)
	{
		auto it = std::find_if(begin(collection), end(collection),
			[id](auto p) {return(p.id_data == id); });

		assert(it != end(collection));

		return (*it);
	}

	template<typename Function>
	std::vector<id> get_entities(Aquarium & aquarium, Function&& function)
	{
		std::vector<id> entities;
		std::for_each(aquarium._types.begin(), aquarium._types.end(),
			[&entities, function](auto p) { if (function(p.living_data.type_data)) { entities.push_back(p.id_data); } });

		return entities;
	}

	const std::vector<id> get_fishs(Aquarium & aquarium)
	{
		return(get_entities(aquarium, is_fish));
	}

	const std::vector<id> get_seaweeds(Aquarium & aquarium)
	{
		return(get_entities(aquarium, is_seaweed));
	}

	const std::vector<id> get_herbivorous(Aquarium & aquarium, std::vector<id> const& fishs)
	{
		std::vector<id> herbivorous;

		for (auto & fish : fishs)
		{
			auto fish_race{ get_component(aquarium._races, fish) };
			if (is_herbivorous_p(fish_race))
			{
				herbivorous.push_back(fish);
			}
		}

		return herbivorous;
	}

	const std::vector<id> get_carnivorous(Aquarium & aquarium, std::vector<id> const& fishs)
	{
		std::vector<id> carnivorous;

		for (auto & fish : fishs)
		{
			auto fish_race{ get_component(aquarium._races, fish) };
			if (is_carnivorous_p(fish_race))
			{
				carnivorous.push_back(fish);
			}
		}

		return carnivorous;
	}

	void delete_entity(Aquarium & aquarium, id id)
	{
		const auto entities_it = std::remove(aquarium._entities.begin(), aquarium._entities.end(), id);
		aquarium._entities.erase(entities_it, aquarium._entities.end());

		const auto details_it = std::remove_if(aquarium._details.begin(), aquarium._details.end(),
			[id](auto p) { return (p.id_data == id); });
		aquarium._details.erase(details_it, aquarium._details.end());

		const auto types_it = std::remove_if(aquarium._types.begin(), aquarium._types.end(),
			[id](auto p) { return (p.id_data == id); });
		aquarium._types.erase(types_it, aquarium._types.end());

		const auto races_it = std::remove_if(aquarium._races.begin(), aquarium._races.end(),
			[id](auto p) { return (p.id_data == id); });
		aquarium._races.erase(races_it, aquarium._races.end());
	}

	void delete_entities(Aquarium & aquarium, std::vector<id> & entities)
	{
		for (auto & entity : entities)
		{
			delete_entity(aquarium, entity);
		}
	}

	void hp_update(Aquarium & aquarium)
	{
		std::vector<id> entity_to_delete;

		for (auto const& entity : aquarium._entities)
		{
			auto & type{ get_component(aquarium._types, entity) };
			if (is_fish_t(type))
			{
				get_hp(type) -= 1;
			}
			else
			{
				get_hp(type) += 1;
			}

			if (is_dead(type)) { entity_to_delete.push_back(entity); }
		}

		delete_entities(aquarium, entity_to_delete);
	}

	void age_update(Aquarium & aquarium)
	{
		std::vector<id> entity_to_delete;

		for (auto const& entity : aquarium._entities)
		{
			auto & type{ get_component(aquarium._types, entity) };

			get_age(type) += 1;

			if (is_hold(type)) { entity_to_delete.push_back(entity); }
		}

		delete_entities(aquarium, entity_to_delete);
	}

	void sexe_update(Aquarium & aquarium, id entity)
	{
		const auto race{ get_component(aquarium._races, entity) };
		auto type{ get_component(aquarium._types, entity) };
		if (is_half_life(type) && is_hermaphrodite_p(race))
		{
			auto & sexe{ get_component(aquarium._details, entity) };
			swap_sexe(sexe);
		}
	}

	void eat_fish(Aquarium & aquarium, living_component & eater_type, id target, std::vector<id> & trash)
	{
		auto & target_type{ get_component(aquarium._types, target) };

		if (!is_dead(target_type))
		{
			get_hp(eater_type) += 5;
			get_hp(target_type) -= 4;

			if (is_dead(target_type))
			{
				trash.push_back(target);
			}
		}
	}

	void eat_seaweed(Aquarium & aquarium, living_component & eater_type, id target, std::vector<id> & trash)
	{
		auto & target_type{ get_component(aquarium._types, target) };

		if (!is_dead(target_type))
		{
			get_hp(eater_type) += 3;
			get_hp(target_type) -= 2;

			if (is_dead(target_type))
			{
				trash.push_back(target);
			}
		}
	}

	void fish_reproduce(Aquarium & aquarium, id fish, id target)
	{
		auto target_type{ get_component(aquarium._types, target) };
		const auto target_race{ get_component(aquarium._races, target) };
		const auto fish_race{ get_component(aquarium._races, fish) };

		if (!is_dead(target_type) && are_both_race(target_race, fish_race))
		{
			auto & fish_detail{ get_component(aquarium._details, fish) };
			auto & target_detail{ get_component(aquarium._details, target) };

			if (!are_both_sexe(fish_detail, target_detail))
			{
				add_fish(aquarium, to_string(fish_race.race_data), fish_race.race_data, random_sexe());
			}
			else if (is_opportunistic_p(target_race))
			{
				swap_sexe(target_detail);
				add_fish(aquarium, to_string(fish_race.race_data), fish_race.race_data, random_sexe());
			}

		}
	}

	void seaweed_reproduce(Aquarium & aquarium, std::vector<id> const& seaweeds)
	{
		for (auto & seaweed : seaweeds)
		{
			auto & seaweed_type{ get_component(aquarium._types, seaweed) };
			auto & seaweed_hp{ get_hp(seaweed_type) };
			if (!is_dead(seaweed_type) && seaweed_hp >= 10)
			{
				seaweed_hp /= 2;
				add_seaweed(aquarium, seaweed_hp);
			}
		}
	}

	void print(size_t tour, size_t entities, size_t seaweed, size_t herbivorous, size_t carnivorous)
	{
		std::cout << tour << "\t" << entities << "\t\t" << seaweed << "\t\t" << herbivorous << "\t\t" << carnivorous << std::endl;
	}

	void fish_spend_time(Aquarium & aquarium, std::vector<id> const& fishs, std::vector<id> const& seaweeds, std::vector<id> & trash)
	{
		std::uniform_int_distribution<size_t> fish_dist(0, fishs.size() - 1);
		std::uniform_int_distribution<size_t> seaweed_dist(0, seaweeds.size() - 1);

		for (auto & fish : fishs)
		{
			sexe_update(aquarium, fish);
			auto & fish_type{ get_component(aquarium._types, fish) };

			if (is_famished(fish_type) && !is_dead(fish_type))
			{
				auto fish_race{ get_component(aquarium._races, fish) };

				if (is_carnivorous(fish_race.race_data) && !fishs.empty())
				{
					eat_fish(aquarium, fish_type, random_entity(fish_dist, fishs), trash);
				}
				else if (is_herbivorous(fish_race.race_data) && !seaweeds.empty())
				{
					eat_seaweed(aquarium, fish_type, random_entity(seaweed_dist, seaweeds), trash);
				}
			}
			else if (!is_famished(fish_type) && !is_dead(fish_type))
			{
				fish_reproduce(aquarium, fish, random_entity(fish_dist, fishs));
			}

		}
	}

	void spend_time(Aquarium & aquarium, std::vector<id> & fishs, std::vector<id> & seaweeds)
	{
		hp_update(aquarium);
		age_update(aquarium);

		fishs = get_fishs(aquarium);
		seaweeds = get_seaweeds(aquarium);

		std::vector<id> entity_dead;

		fish_spend_time(aquarium, fishs, seaweeds, entity_dead);

		seaweed_reproduce(aquarium, seaweeds);

		delete_entities(aquarium, entity_dead);
	}
}

int main() {

	ecs::Aquarium aquarium;

	for (size_t i{}; i < 5; i++) { ecs::add_seaweed(aquarium); };
	for (size_t i{}; i < 5; i++)
	{
		const auto race{ ecs::random_race() };
		ecs::add_fish(aquarium, ecs::to_string(race), race, ecs::random_sexe());
	}

	std::cout << "Tourt\tEntitees\tAlgue\tHerbivores\tCarnivores" << std::endl;

	for (size_t tour{}; tour < 20; ++tour)
	{
		auto fishs{ ecs::get_fishs(aquarium) };
		auto seaweeds{ ecs::get_seaweeds(aquarium) };
		const auto herbivorous{ ecs::get_herbivorous(aquarium, fishs) };
		const auto carnivorous{ ecs::get_carnivorous(aquarium, fishs) };

		ecs::print(tour, aquarium._entities.size(), seaweeds.size(), herbivorous.size(), carnivorous.size());

		ecs::spend_time(aquarium, fishs, seaweeds);


	}

	std::cout << "===== Time Out ===== " << std::endl;

	return 0;
}