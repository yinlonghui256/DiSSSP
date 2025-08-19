#include "../ManualLinkedList.h"


int main() {
    auto base = std::make_shared<ManualLinkedListBase>(10);
    base->debugPrint();

    auto list1 = base->newList();
    list1.add(1);
    list1.add(2);

	// now: list 1: 2 -> 1; id : 10.
    list1.debugPrint();
    base->debugPrint();

    auto list2 = base->newList();
    list2.add(1);
    list2.add(4);

	// now: list 1: 2; id: 10. list 2: 4 -> 1, id: 11.
    list1.debugPrint();
    list2.debugPrint();
    base->debugPrint();

    auto list3 = std::make_shared<ManualLinkedList>(base->newList());

    list3 -> add(3);
    list3 -> add(5);
    list3 -> add(1);
    
	// now: list 1: 2; id: 10. list 2: 4; id: 11. list 3: 1 -> 5 -> 3, id: 12.
    list1.debugPrint();
    list2.debugPrint();
    list3->debugPrint();
    base->debugPrint();

    list3.reset();
    
	// now: list 1: 2; id: 10. list 2: 4; id: 11. blockPool: 12
    base->debugPrint();

	auto list4 = std::make_shared<ManualLinkedList>(base->newList());
	auto list5 = std::make_shared<ManualLinkedList>(base->newList());
    list3 = std::make_shared<ManualLinkedList>(base->newList());

	list4->add(6);
    list4->add(7);
    list5->add(8);
    list5->add(9);
	list3->add(0);

	// now: list 1: 2; id: 10. list 2: 4; id: 11. list 3: 0; id: 14. list 4: 7 -> 6; id: 12. list 5: 9 -> 8, id: 13.
    list1.debugPrint();
    list2.debugPrint();
    list3->debugPrint();
	list4->debugPrint();
	list5->debugPrint();
    base->debugPrint();

	list1.merge(list2);
	list2.merge(*list4); // merging an nonempty list into an empty list, would cause their id to be swapped.
	list3->merge(*list5);

	// now: list 1: 2 -> 4; id: 10. list 2: 7 -> 6; id: 12. list 3: 0 -> 9 -> 8; id: 14. list 4: empty; id: 11. list 5: empty, id: 13.
    list1.debugPrint();
    list2.debugPrint();
    list3->debugPrint();
    list4->debugPrint();
    list5->debugPrint();
    base->debugPrint();

    list4.reset();
    list5.reset();
	auto list6 = std::make_shared<ManualLinkedList>(base->newList());

	// now: blockPool: 11; list 6: empty; id: 13.
    list1.debugPrint();
    list2.debugPrint();
    list3->debugPrint();
	list6->debugPrint();
    base->debugPrint();
}
