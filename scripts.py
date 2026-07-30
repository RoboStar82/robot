
Import("env")

env.AddCustomTarget(
    name="scripts",
    dependencies=None,
    actions=[
        "echo 'Upload scripts...'"
    ],
    title="Upload scripts",
    description="Upload scripts"
)
