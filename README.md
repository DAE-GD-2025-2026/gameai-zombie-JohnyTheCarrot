# Research project

URL: https://github.com/DAE-GD-2025-2026/gameai-research-project-JohnyTheCarrot

## Research topic

Goal Oriented Action Planning (GOAP)

## Research sources

- https://www.reddit.com/r/godot/comments/xgrk0g/comment/iote6ft/?utm_source=share&utm_medium=web3x&utm_name=web3xcss&utm_term=1&utm_content=share_button
- https://www.youtube.com/watch?v=nEnNtiumgII
- https://goap.crashkonijn.com/readme/tutorial/gettingstarted

## Brief

Goal Oriented Action Planning works through a current 'world state', and the target state you want it to become, i.e. your Goal.
Goals can be achieved through Actions.

### Example:
Initial state: Hungry

We want to NOT be hungry. (Goal State: NotHungry)

Actions could be eating an apple. (Preconditions: Hungry, HasApple) (Effects: +NotHungry, -HasApple)

Fulfulling HasApple can be done by perhaps buying one from a merchant, or plucking one yourself.
The choice between the two is made through a cost. The apple tree plucking may cost energy, but a merchant will charge money. This consideration changes with the amount of energy and money we have.
Each action applies its outcome to the previous state. This allows for simulation of actions, which allows us to plan out the use of actions to achieve our goal using pathfinding algorithms such as A*, which is what I used.

## Scope

This research project is part of my zombie game, though the zombie game itself is far from on point.
The focus lies on the GOAP algorithm. The survivor will not always act optimally. He does not currently run away from danger, but he does shoot enemies if he can.

The application of GOAP to this project is essentially a proof of concept, to be further developed in the final zombie game hand in, which it would reflect.
The goal of this specific handed in version of the zombie game is solely to test GOAP, but it is not perfectly used, as mentioned.
An example of this compromise is the fact the survivor currently immediately points towards an enemy, instantly shoots.
The goal of this, again, is to research GOAP.
It may not always get the best results in this MVP, depending on the seed. You can always try another seed.

## GOAP Implementation

The zombie game has a tendency to change at rather short notice. A zombie would come into the picture very suddenly, meaning re-planning is inherent.
With this in mind, I didn't implement too crazy look-ahead calculations, since replanning happens quite often in any case.
Performance doesn't seem affected, at least not to a degree I could pick up on.

One thing you may also notice, is that the effects on Actions are rather optimistic and/or naive.
Picking up a weapon would have the effect of now no longer being aware of any weapons around, which isn't necessarily the case.
With each state change, I check whether we're still on track to our goal. A violation of this naive assumption is essentially corrected at this stage.

As the ai controller perceives certain Actors such as items, houses, enemies, I take note of them.
There's a numeric state, which in and of itself does not necessarily directly affect the GOAP state.

The GAOP state, in the code base referred to as the GOAP flags, are a simple bit field of flags, such as HasWeapon. I.e. true/false.
These flags are set by checking the numeric state. The advantage is that if the numeric state changes but does not trigger a flag change, we can just proceed as normal.
It also simplifies GOAP use.

Example:

```
GoapComp->State.AwareOf.WeaponsNum = KnownWeapons.Num();
// [...]
SetFlag(EGOAPFlags_Martens_Tuur::HasFoundWeapon, AwareOf.WeaponsNum > 0);
```

## GOAP Use/Usage

Both actions and goals are defined in data assets:

<img width="1290" height="391" alt="image" src="https://github.com/user-attachments/assets/8bd8344f-8f45-4ea8-b6c9-73028896c7a2" />
<img width="1003" height="380" alt="image" src="https://github.com/user-attachments/assets/c2bdee96-88ad-42fb-850c-4dc54828ba46" />

Has Free Inventory Slots is another example of naive assumptions that get corrected, but overall help with planning
in this case it is meant to help a goal give the agent something to do if other goals are met.
<img width="1169" height="663" alt="image" src="https://github.com/user-attachments/assets/80eb3692-0410-4564-a613-8879e86d7bc6" />

The actual logic of an action is defined through its Executor, which is implemented in C++.
An executor has a status (Busy, Success, Failure). Busy means not to proceed to the next action. Failure means inherent re-evaluation of the plan. Success means proceed onwards.

## Plans

Since I plan to use this for my zombie game hand-in, I hope to further clean up some dead code.
Of course I will also be making further changes to improve the behavior of the agent, such as allowing sprinting, running away.
