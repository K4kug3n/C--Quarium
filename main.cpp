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
	using entities = std::vector<id>;

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
		std::default_random_engine _random_engine;
		entities _entities;
		details _details;
		types _types;
		races _races;
	};


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

	using iterator = entities::iterator;
	using reverse_iterator = entities::reverse_iterator;

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
		const std::string races[]{ "Mérou", "Thon", "Clown", "Sole", "Bar", "Carpe" };
		return races[static_cast<size_t>(r)];
	}

	bool random_sexe(Aquarium & aquarium)
	{
		std::uniform_int_distribution<size_t> distribution(0, 1);

		return(distribution(aquarium._random_engine) == 0);
	}

	race random_race(Aquarium & aquarium)
	{
		std::uniform_int_distribution<size_t> distribution(0, 5);
		return static_cast<race>(distribution(aquarium._random_engine));
	}

	iterator random_iterator(Aquarium & aquarium, std::uniform_int_distribution<size_t> & dist, entities & entities, iterator exclusion)
	{
		iterator it;
		do {
			it = entities.begin() + dist(aquarium._random_engine);
		} while (it == exclusion);

		return it;
	}

	id random_entity(Aquarium & aquarium, std::uniform_int_distribution<size_t> & dist, entities const& entities)
	{
		return entities[dist(aquarium._random_engine)];
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
	entities get_entities(Aquarium & aquarium, Function&& function)
	{
		entities entities;
		std::for_each(aquarium._types.begin(), aquarium._types.end(),
			[&entities, function](auto p) { if (function(p.living_data.type_data)) { entities.push_back(p.id_data); } });

		return entities;
	}

	const entities get_fishs(Aquarium & aquarium)
	{
		return(get_entities(aquarium, is_fish));
	}

	const entities get_seaweeds(Aquarium & aquarium)
	{
		return(get_entities(aquarium, is_seaweed));
	}

	const entities get_herbivorous(Aquarium & aquarium, entities const& fishs)
	{
		entities herbivorous;

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

	const entities get_carnivorous(Aquarium & aquarium, entities const& fishs)
	{
		entities carnivorous;

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

	void delete_entity(entities & collection, iterator & actual_place, iterator & target)
	{
		std::swap(*target, *(collection.rbegin()));
		if (std::distance(actual_place, target) >= 0)
		{
			collection.resize(collection.size() - 1);
		}
		else
		{
			std::swap(*target, *(collection.rbegin()));
		}
	}

	void delete_entities(Aquarium & aquarium, entities & entities)
	{
		for (auto & entity : entities)
		{
			delete_entity(aquarium, entity);
		}
	}

	void hp_update(Aquarium & aquarium)
	{
		entities entity_to_delete;

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
		entities entity_to_delete;

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
				add_fish(aquarium, to_string(fish_race.race_data), fish_race.race_data, random_sexe(aquarium));
			}
			else if (is_opportunistic_p(target_race))
			{
				swap_sexe(target_detail);
				add_fish(aquarium, to_string(fish_race.race_data), fish_race.race_data, random_sexe(aquarium));
			}

		}
	}

	void eat_seaweed(Aquarium & aquarium, living_component & eater_type, id target, entities & seaweeds)
	{
		auto & target_type{ get_component(aquarium._types, target) };

		if (!is_dead(target_type))
		{
			get_hp(eater_type) += 3;
			get_hp(target_type) -= 2;

			if (is_dead(target_type))
			{
				delete_entity(aquarium, target);
				seaweeds.erase(std::remove(seaweeds.begin(), seaweeds.end(), target), seaweeds.end());
			}
		}
	}

	bool need_update(iterator active, iterator target)
	{
		return std::distance(active, target) < 0;
	}

	void fish_eat_fish(Aquarium & aquarium, iterator active, iterator & target,
		reverse_iterator & last_valid, entities & fishs, entities & seaweeds);

	void update_fish(Aquarium & aquarium, iterator active, reverse_iterator & last_valid,
		entities & fishs, entities & seaweeds)
	{
		std::uniform_int_distribution<size_t> seaweed_dist(0, seaweeds.size() - 1);
		std::uniform_int_distribution<size_t> fish_dist(0, fishs.size() - 1);

		sexe_update(aquarium, *active);
		auto & fish_type{ get_component(aquarium._types, *active) };
		if (is_famished(fish_type))
		{
			auto fish_race{ get_component(aquarium._races, *active) };
			if (is_carnivorous(fish_race.race_data) && (fishs.size() - std::distance(last_valid, fishs.rbegin())) > 1) {
				fish_eat_fish(aquarium, active, random_iterator(aquarium, fish_dist, fishs, active), last_valid, fishs, seaweeds);
			}
			else if (is_herbivorous(fish_race.race_data) && !seaweeds.empty()) {
				eat_seaweed(aquarium, fish_type, random_entity(aquarium, seaweed_dist, seaweeds), seaweeds);
			}

		}
		else if (!is_famished(fish_type))
		{
			fish_reproduce(aquarium, *active, random_entity(aquarium, fish_dist, fishs));
		}

	}

	void update_fishs(Aquarium & aquarium, iterator begin, reverse_iterator & last_valid,
		entities & fishs, entities & seaweeds)
	{
		for (iterator active{ begin }; *active != *last_valid; ++active)
		{
			update_fish(aquarium, active, last_valid, fishs, seaweeds);
		}
	}

	void eat_fish(Aquarium & aquarium, iterator active, iterator & target,
		reverse_iterator & lastValid)
	{

		auto & eater_type{ get_component(aquarium._types, *active) };
		auto & target_type{ get_component(aquarium._types, *target) };

		get_hp(eater_type) += 5;
		get_hp(target_type) -= 4;

		if (is_dead(target_type)) {

			delete_entity(aquarium, *target);

			std::swap(*target, *lastValid);
			++lastValid;

		}
	}

	void fish_eat_fish(Aquarium & aquarium, iterator active, iterator & target,
		reverse_iterator & last_valid, entities & fishs, entities & seaweeds)
	{
		eat_fish(aquarium, active, target, last_valid);

		if (need_update(active, target))
		{
			update_fish(aquarium, target, last_valid, fishs, seaweeds);
		}
	}

	void fish_spend_time(Aquarium & aquarium, entities & fishs, entities & seaweeds)
	{
		reverse_iterator last_valid = fishs.rbegin();
		update_fishs(aquarium, fishs.begin(), last_valid, fishs, seaweeds);
		auto sizeToRem = std::distance(fishs.rbegin(), last_valid);
		fishs.resize(fishs.size() - sizeToRem);
	}

	void seaweed_reproduce(Aquarium & aquarium, entities const& seaweeds)
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

	void spend_time(Aquarium & aquarium, entities & fishs, entities & seaweeds)
	{
		hp_update(aquarium);
		age_update(aquarium);

		fishs = get_fishs(aquarium);
		seaweeds = get_seaweeds(aquarium);

		fish_spend_time(aquarium, fishs, seaweeds);

		seaweed_reproduce(aquarium, seaweeds);

	}
}

int main() {

	ecs::Aquarium aquarium{ std::default_random_engine{} };

	for (size_t i{}; i < 5; i++) { ecs::add_seaweed(aquarium); };
	for (size_t i{}; i < 5; i++)
	{
		const auto race{ ecs::random_race(aquarium) };
		ecs::add_fish(aquarium, ecs::to_string(race), race, ecs::random_sexe(aquarium));
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