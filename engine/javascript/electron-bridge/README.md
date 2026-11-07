# JavaScript Electron Bridge

This layer will connect the Kalya desktop application's Electron main process
to the KalyaDB client API.

Keep database operations behind a narrow, validated boundary. Do not expose
arbitrary filesystem access to renderer code, and do not duplicate storage
logic in JavaScript.
