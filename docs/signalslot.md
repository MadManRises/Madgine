# Signals & Slots

The Signal & Slots implementation of the Madgine is inspired by the implementation in Qt, which also coined the name for that concept. It allows signals to be defined anywhere in the program. Those signals can be connected to slots at runtime. Whenever a signal is emitted, all connected slots are invoked. Signals can have arbitrary arguments, that will be passed to the slots. There are two types of connections. Direct connections will be invoked immediately, when the signal is emitted. Queued connections will be added to the the global task queue (see [Event Handling][])


[Event Handling]: event.md